#pragma once

#include <QtCore>
#include <vector>
#include <bearparser.h>

#include "ExeDisasm.h"
#include "CodeBlock.h"

#define MAX_UNSOLVED 200 // maximal number of unsolved addresses that can be autotraced

namespace minidis {

class Tracer : public QObject, public AddrConverter
{
    Q_OBJECT

signals:
    void loadingProgress(int progress);

public:
    Tracer(Executable *exe) : QObject(), AddrConverter(),
        m_Exe(exe),
        m_nameManager(this), m_funcManager(this), m_impFuncManager(this)
    {
        m_bitMode = m_Exe->getBitMode();
    }

    virtual ~Tracer() {}

    bool resolveOffset(offset_t offset, Executable::addr_type aType);

    QString getStringAt(offset_t target);

    /* conversion */
    virtual offset_t convertAddr(offset_t off, Executable::addr_type inType, Executable::addr_type outType) const
    {
        return m_Exe->convertAddr(off, inType, outType);
    }
    
    virtual Executable::addr_type detectAddrType(offset_t off, Executable::addr_type hintType) const
    {
        return m_Exe->detectAddrType(off, hintType);
    }

    virtual ExeDisasm* getDisasmAt(offset_t offset, Executable::addr_type inType = Executable::RAW) const
    {
        offset = this->convertAddr(offset, inType, Executable::RAW);
        if (m_offsetToDisasm.contains(offset)) {
            return m_offsetToDisasm[offset];
        }
        if (m_disasmPtrs.contains(offset)) {
            return m_disasmPtrs[offset];
        }
        // closest 
        QMap<offset_t, ExeDisasm*>::const_iterator disItr = this->m_offsetToDisasm.begin();//upperBound(offset);
        for (; disItr != this->m_offsetToDisasm.constEnd() ; disItr++) {
            ExeDisasm* dis = disItr.value();
            if (dis->hasOffset(offset)) {
                return dis;
            }
        }
        return NULL;
    }
    
    virtual minidis::mnem_type getMnemTypeAtOffset(offset_t offset, Executable::addr_type inType)
    {
        offset = this->convertAddr(offset, inType, Executable::RAW);
        ExeDisasm* dis = getDisasmAt(offset);
        if (!dis) return MT_OTHER;
        return dis->getMnemTypeAtOffset(offset, inType);
    }

    QString getHexString(offset_t offset, Executable::addr_type aType)
    {
        offset = this->convertAddr(offset, aType, Executable::RAW);
        ExeDisasm* dis = getDisasmAt(offset);
        if (!dis) return NULL;

        size_t index = dis->m_disasmBuf.offsetToIndex(offset);
        if (index == INVALID_INDEX) return "?";

        DisasmChunk *chunk = dis->getChunkAtIndex(index);
        if (!chunk) return ":(";
        return dis->getChunkAtIndex(index)->toHexString();
    }

    QString translateBranching(ExeDisasm* dis, const size_t index, FuncNameManager *nameManager) const;

    virtual QString getDisasmString(offset_t offset, Executable::addr_type aType)
    {
        offset = this->convertAddr(offset, aType, Executable::RAW);
        ExeDisasm* dis = getDisasmAt(offset);
        if (!dis) return NULL;

        size_t index = dis->m_disasmBuf.offsetToIndex(offset);
        if (index == INVALID_INDEX) return "?";

        QString str;
        bool isBranching = dis->isBranching(offset, Executable::RAW);
        const offset_t target = dis->getTargetOffset(index, Executable::RAW);

        if (isBranching) {
            str = translateBranching(dis, index, &this->m_nameManager);
        } else {
            str = dis->m_disasmBuf.getDisasmString(index);
        }
        if (isImportedFunction(target, Executable::RAW)) {
            return str + " -> " + getImportName(target, Executable::RAW);
        }
        
        if (!isBranching && this->hasName(target, Executable::RAW)) {
            return str + " -> " + this->getFunctionName(target, Executable::RAW);
        }

        return str;
    }

    virtual bool isInternalCall(offset_t offset, Executable::addr_type inType);
    virtual offset_t getTargetOffset(offset_t offset, Executable::addr_type inType,  Executable::addr_type outType)
    {
        offset = this->convertAddr(offset, inType, Executable::RAW);
        ExeDisasm* dis = getDisasmAt(offset);
        if (!dis) return INVALID_ADDR;

        size_t index = dis->m_disasmBuf.offsetToIndex(offset);
        if (index == INVALID_INDEX) return INVALID_ADDR;

        return dis->getTargetOffset(index, outType);
    }

    virtual bool isBranching(offset_t offset, Executable::addr_type inType) const {
        offset = this->convertAddr(offset, inType, Executable::RAW);
        ExeDisasm* dis = getDisasmAt(offset);
        if (!dis) return false;

        return dis->isBranching(offset, Executable::RAW);
    }

    virtual bool isFollowable(offset_t offset, Executable::addr_type inType) const {
        offset = this->convertAddr(offset, inType, Executable::RAW);
        ExeDisasm* dis = getDisasmAt(offset);
        if (!dis) return false;

        return dis->isFollowable(offset, Executable::RAW);
    }

    virtual bool isImportedFunction(offset_t offset, Executable::addr_type aType) const = 0;
    virtual QString getImportName(offset_t offset, Executable::addr_type aType) const = 0;

    QList<offset_t> forksList;
    QList<offset_t>& blocksList() { return this->m_validBlocksOffsets; }
   
    CodeBlock* blockStartingAt(offset_t offset, Executable::addr_type aType = Executable::RAW)
    {
        const offset_t raw = this->convertAddr(offset, aType, Executable::RAW); 
        return m_blocks.contains(raw) ? &m_blocks[raw] : NULL;
    }

    CodeBlock* blockAt(offset_t offset, Executable::addr_type aType = Executable::RAW)
    {
        offset_t raw = this->convertAddr(offset, aType, Executable::RAW);
        //give priority to block starting at given offset
        CodeBlock *block = blockStartingAt(offset, aType);
        if (block) return block;
        return m_blockPtrs.contains(raw) ? m_blockPtrs[raw] : NULL;
    }

    ForkPoint* forkAt(offset_t raw) { return m_forks.contains(raw) ? &m_forks[raw] : NULL; }

    QSet<offset_t>* refsTo(offset_t offset, Executable::addr_type aType)
    {
        offset_t raw = convertAddr(offset,  aType, Executable::RAW);
        return m_refs.contains(raw) ? &m_refs[raw] : NULL;
    }

    QSet<CodeBlock*>* refBlocksTo(offset_t offset, Executable::addr_type aType)
    {
        offset_t raw = convertAddr(offset,  aType, Executable::RAW);
        return m_refBlocks.contains(raw) ? &m_refBlocks[raw] : NULL;
    }

    bool hasName(offset_t offset, Executable::addr_type inType) const
    {
        return m_nameManager.hasName(offset, inType);
    }

    QString getFunctionName(offset_t offset, Executable::addr_type inType) const
    {
        return m_nameManager.getFunctionName(offset, inType);
    }

    bool hasReferedString(offset_t offset, Executable::addr_type inType) const
    {
        offset = convertAddr(offset, inType, Executable::RAW);
        return (this->m_referedStrings.contains(offset));
    }

    QString getReferedString(offset_t offset, Executable::addr_type inType) const
    {
        offset = convertAddr(offset, inType, Executable::RAW);
        if (!this->m_referedStrings.contains(offset)) return "?";

        return m_referedStrings[offset];
    }
    virtual void traceEntrySection() = 0;
    virtual bool traceFunction(offset_t offset, Executable::addr_type aType, QString name, bool stopAtBlockEnd = true) = 0;
    virtual bool defineFunction(offset_t offset, Executable::addr_type aType, QString name);

    QList<offset_t>& getFunctionsList() { return m_funcManager.list(); }
    QList<offset_t>& getImpFunctionsList() { return m_impFuncManager.list(); }
    QList<offset_t>& getReferedStringsList() { return m_referedStringsList; }
    QList<offset_t>& getNamedOffsetsList() { return m_nameManager.getNamedOffsetsList(); }

    bool resolveUnsolved(const size_t maxDepth = 1)
    {
        emit loadingProgress(0);
        int progress = 0;
        int progressChunk = 100 / maxDepth;

        size_t depth = 0;
        QSet<offset_t> unsolved = m_unsolvedOffsets;
        while (depth++ < maxDepth) {
            fetchUnsolved(unsolved);
            std::cout <<"Depth: " << std::dec << depth << " unsolved : " << unsolved.size() << std::endl;
            if (unsolved.size() == 0) {
                return true;
            }
            if (unsolved.size() > MAX_UNSOLVED) {
                std::cout << "Unsolved limit exceeded: " << std::dec << unsolved.size() << std::endl;
                break;
            }
            resolveUnsolvedBranches(unsolved);
            unsolved.clear();
            progress += progressChunk;
            emit loadingProgress(progress);
        }
        return false;
    }
    
    size_t saveFunctionNames(const QString &fileName) { return m_nameManager.save(fileName); }
    size_t loadFunctionNames(const QString &fileName) { return m_nameManager.load(fileName); }

    bool setSingleFunctionName(offset_t offset, Executable::addr_type inType, QString name)
    {
        if (offset == this->m_Exe->getEntryPoint(inType) && hasName(offset, inType)) {
            return false; // do not change name at EP
        }
        return m_nameManager.setFunctionName(offset, inType, name);
    }

protected:

    bool appendCodeChunk(ExeDisasm* disasm, CodeBlock* block, const offset_t currOff);

    virtual ExeDisasm* makeDisasm(Executable* exe, offset_t startRaw) = 0;
    size_t fetchUnsolved(QSet<offset_t> &unresolvedSet);
    virtual void resolveUnsolvedBranches(const QSet<offset_t> &unsolved);

    bool makeDisasmAt(Executable* exe, offset_t raw, bool stopAtBlockEnd, size_t maxElements = DEFAULT_MAX_EL);
    
    void addReferencedTargets(offset_t currOffset, offset_t target, DisasmChunk *chunk);
    void traceArea(offset_t start);

    /* references */
    void addReference(offset_t offset, offset_t calledAt)
    {
        if (offset == INVALID_ADDR || calledAt == INVALID_ADDR) return;
        m_refs[offset].insert(calledAt);
    }

    void addForkPoint(offset_t currOff, offset_t target, offset_t next);

    void traceBlocks(ExeDisasm* disasm, offset_t startOffset);
    CodeBlock* addNewCodeBlock(offset_t offset);
    void filterValidBlocks();
    void traceReferencedCodeBlocks();

    FuncNameManager m_nameManager;
    FuncManager m_funcManager;
    FuncManager m_impFuncManager;

    // basic keepers
    QMap<offset_t, ExeDisasm*> m_offsetToDisasm;
    QMap<offset_t, CodeBlock> m_blocks;
    QMap<offset_t, QSet<offset_t> > m_refs;
    QMap<offset_t, ForkPoint> m_forks;
    QSet<offset_t> m_unsolvedOffsets;
    QMap<offset_t, QString> m_referedStrings;

    // helpers for faster access
    QList<offset_t> m_referedStringsList;
    QList<offset_t> m_validBlocksOffsets;
    QSet<offset_t> m_validBlocksOffsetsSet;

    QMap<offset_t, ExeDisasm*> m_disasmPtrs;
    QMap<offset_t, CodeBlock*> m_blockPtrs;
    QMap<offset_t, QSet<CodeBlock*> > m_refBlocks;

    Executable::exe_bits m_bitMode;
    Executable *m_Exe;
    //offset_t m_start;

}; /* class Tracer */

}; /* namespace minidis */
