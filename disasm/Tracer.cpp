#include "Tracer.h"

using namespace minidis;

#include "ExeDisasm.h"

bool Tracer::resolveOffset(offset_t offset, Executable::addr_type aType)
{
    offset_t raw = this->convertAddr(offset, aType, Executable::RAW);
    if (this->getDisasmAt(raw, Executable::RAW)) return true;

    if (!this->makeDisasmAt(m_Exe, raw, this->functionTraceSettings)) {
        return false; //FAILED
    }
    this->traceArea(raw);
    return true;
}

QString Tracer::getStringAt(offset_t target)
{
    QString str = this->m_Exe->getStringValue(target);
    if (str.size() == 1) {
        str = this->m_Exe->getWAsciiStringValue(target, 100);
    }
    if (str.trimmed().length() == 0) return "";
    return str;
}

CodeBlock* Tracer::getOrMakeCodeBlockAt(offset_t offset)
{
    if (offset == INVALID_ADDR) return NULL;
    
    if (!this->getDisasmAt(offset)) {
        m_unsolvedOffsets.insert(offset);
        //printf("UNSOLVED at = %x\n", offset);
        return NULL;
    }
    if (m_blocks.contains(offset)) {
        return &m_blocks[offset];
    }
    m_blocks[offset] = CodeBlock(offset);
    m_blockPtrs[offset] = &m_blocks[offset];
    //printf("Adding new block at = %x\n", offset);
    return  &m_blocks[offset];
}

size_t Tracer::fetchUnsolved(QSet<offset_t> &unsolved)
{
    size_t added = 0;
    for (QMap<offset_t, ForkPoint>::iterator itr = m_forks.begin();itr != m_forks.end(); ++itr) {
        ForkPoint &p = itr.value();
        offset_t uOff = p.yesOffset;
        if (uOff == INVALID_ADDR) continue;
        if (!getDisasmAt(uOff)) {
            unsolved.insert(uOff);
            added++;
        }
    }
    QList<offset_t> func = m_funcManager.list();
    for (QList<offset_t>::iterator itr = func.begin();itr != func.end(); ++itr) {
        offset_t uOff = *itr;
        uOff = this->convertAddr(uOff, m_funcManager.getAddrType(), Executable::RAW);
        if (uOff == INVALID_ADDR) continue;
        if (!getDisasmAt(uOff)) {
            unsolved.insert(uOff);
            added++;
        }
    }
    return added;
}

size_t Tracer::resolveUnsolvedBranches(const QSet<offset_t> &unsolved)
{
    size_t added = 0;
    QSet<offset_t>::const_iterator oItr;
    for (oItr = unsolved.begin();oItr != unsolved.end(); ++oItr) {
        offset_t uOff = *oItr;
        if (uOff == INVALID_ADDR) continue;
        if (!getDisasmAt(uOff)) {
            if (this->makeDisasmAt(m_Exe, uOff, this->functionTraceSettings)) {
                traceArea(uOff);
                added++;
            }
        }
    }
    return added;
}

void Tracer::addReferencedTargets(offset_t currOff, offset_t target, DisasmChunk *chunk)
{
    if (chunk == NULL || currOff == INVALID_ADDR || target == INVALID_ADDR) return;

    bool import = this->isImportedFunction(target, Executable::RAW);
    if (import) {
        if (m_impFuncManager.appendFunction(target, Executable::RAW)) {
            addReference(target, currOff);
        }
        return;
    }
    const mnem_type mType = chunk->getMnemType();
    if (mType == MT_CALL) {
        //local function called
        if (m_funcManager.appendFunction(target, Executable::RAW)) {
            addReference(target, currOff);
        }
        return;
    }

    if (DisasmBase::isBranching(mType)) return;
    if (m_referedStrings.contains(target)) {
        addReference(target, currOff);
    } else {
        QString str = this->getStringAt(target);
        if (str.length() > 0) {
            m_referedStrings[target] = str;
            this->m_referedStringsList.append(target);
            addReference(target, currOff);
        }
    }
}

bool Tracer::appendCodeChunk(DisasmBase* disasm, CodeBlock* block, const offset_t currOff)
{
    if (!disasm || !block || currOff == INVALID_ADDR) return false;
    m_disasmPtrs[currOff] = disasm;

    if (!block->append(currOff)) {
        //printf("Cannot append to block\n");
        return false;
    }
    m_blockPtrs[currOff] = block;
    return true;
}

void Tracer::addForkPoint(offset_t currOff, offset_t target, offset_t next)
{
    m_forks[currOff] = ForkPoint(currOff, target, next);
    addReference(target, currOff);
    addReference(next, currOff);
}

void Tracer::traceBlocks(DisasmBase* disasm, offset_t startOffset)
{
    if (!disasm) return;

    const Executable::addr_type aType = Executable::RAW;

    const size_t startIndex = disasm->m_disasmBuf.offsetToIndex(startOffset);
    const size_t disasmSize = disasm->m_disasmBuf.size();

    const size_t initialBlockCount = m_blocks.size();

    offset_t newOffset = startOffset;
    CodeBlock* block = NULL;

    const bool skipExisting = true;
    for (size_t index = startIndex; index < disasmSize; index++)
    {
        if (!block) {
            CodeBlock *existingBlock = this->blockStartingAt(newOffset, Executable::RAW);
            if (existingBlock) {
                if (!skipExisting) {
                    std::cout <<std::hex << "[WARNING][" << newOffset << "] A block starting at this offset already exist!\n";
                    break;
                }
                offset_t endOffset = existingBlock->getEndOffset();
                if (endOffset == INVALID_ADDR) break;
                
                int lastIndx = existingBlock->getIndexOf(endOffset);
                if (lastIndx == INVALID_INDEX) break;
                newOffset = disasm->getNextOffset(index);
                //std::cout << "Skipping to next offset: " << std::hex << newOffset << "\n";
                continue;
            }
            block = getOrMakeCodeBlockAt(newOffset);
            if (!block) break;
            //std::cout << "New block: " << std::hex << newOffset << std::endl;
        }
        DisasmChunk *chunk = disasm->getChunkAtIndex(index);
        if (!chunk) break;

        const offset_t currOff = disasm->getOffset(index);
        if (currOff == INVALID_ADDR) {
            break;
        }
        mnem_type mType = disasm->getMnemTypeAtIndex(index);
        offset_t target = disasm->getTargetOffset(index, Executable::RAW);
        offset_t targetRVA = disasm->getTargetOffset(index, Executable::RVA);

        if (!appendCodeChunk(disasm, block, currOff)) {
            //printf("Could not append code chunk at: %lx\n", currOff);
            break;
        }

        if (mType == MT_INVALID) {
            block->markInvalid();
        }

        if (target != INVALID_ADDR) {
            addReferencedTargets(currOff, target, chunk);
            if (disasm->isPushRet(index)) {
                //local function called
                if (m_funcManager.appendFunction(target, Executable::RAW)) {
                    addReference(target, currOff);
                }
            }
        }

        if (!disasm->isBlockEnd(mType)) {
            //the block is not finished yet, process further chunks
            continue;
        }
        //block finished:
        if (disasm->isBranching(index) && target == INVALID_ADDR) {
            block->markInvalid();
        }

        if (disasm->isBranching(index) && !block->isInvalid()) {
            bool import = this->isImportedFunction(target, Executable::RAW);

            if (disasm->isConditionalBranching(mType)) {
                const offset_t next = disasm->getNextOffset(index);
                addForkPoint(currOff, target, next);

            } else if (!isImportedFunction(target, Executable::RAW)) {
                //add jump fork point only if it is not a jump to import
                addForkPoint(currOff, target, INVALID_ADDR);
            }
        }
        if (block->size == 0) {
            if (mType == MT_INT3) block->markInvalid();
        }
        newOffset = disasm->getNextOffset(index);
        //reset block:
        block = NULL;
    }
    
    if (initialBlockCount == m_blocks.size()){
        std::cout << "No blocks added!\n";
        return;
    }
    filterValidBlocks();
}

void Tracer::traceArea(offset_t startOffset)
{
   if (!m_offsetToDisasm.contains(startOffset)) {
        return;
   }
    DisasmBase* disasm = this->getDisasmAt(startOffset);
    if (!disasm) return;
    //printf("Got disasm at startOffset = %lx ,ptr =%x\n", startOffset, disasm);
    traceBlocks(disasm, startOffset);
    traceReferencedCodeBlocks();
}

void Tracer::filterValidBlocks()
{
    //block addr type: RAW
    QMap<offset_t, CodeBlock>::iterator itr;
    for (itr = this->m_blocks.begin(); itr != m_blocks.end(); ++itr) {

        offset_t offset = itr.key();
        CodeBlock &block = itr.value();
        int size = m_validBlocksOffsetsSet.size();

        if (!block.isInvalid()) { // valid
            m_validBlocksOffsetsSet.insert(offset);
        }
        // if offset is new, append it!
        if (size < m_validBlocksOffsetsSet.size()) {
            this->m_validBlocksOffsets.append(offset);
        }
    }
}

void Tracer::traceReferencedCodeBlocks()
{
    QMap<offset_t, QSet<offset_t> >::iterator itr;
    for (itr = this->m_refs.begin(); itr != m_refs.end(); ++itr ) {
        offset_t offset = itr.key();
        QSet<offset_t> &rSet = itr.value();
        QSet<offset_t>::iterator itrR;

        for (itrR = rSet.begin(); itrR != rSet.end(); ++itrR ) {
            CodeBlock* block = this->blockAt(*itrR);
            if (!block) continue;
            this->m_refBlocks[offset].insert(block);
        }
    }
}

DisasmBase* Tracer::getDisasmAt(offset_t offset, Executable::addr_type inType) const
{
    offset = this->convertAddr(offset, inType, Executable::RAW);
    if (m_offsetToDisasm.contains(offset)) {
        return m_offsetToDisasm[offset];
    }
    if (m_disasmPtrs.contains(offset)) {
        return m_disasmPtrs[offset];
    }
    // closest 
    QMap<offset_t, DisasmBase*>::const_iterator disItr = this->m_offsetToDisasm.begin();//upperBound(offset);
    for (; disItr != this->m_offsetToDisasm.constEnd(); ++disItr) {
        DisasmBase* dis = disItr.value();
        if (dis->hasOffset(offset)) {
            return dis;
        }
    }
    return NULL;
}

bool Tracer::makeDisasmAt(Executable* exe, offset_t offset, DisasmSettings &settings)
{
    //printf("Disasm making at %x\n", offset);
    DisasmBase *disasm = getDisasmAt(offset);
    if (!disasm) {
       disasm = makeDisasm(exe, offset);
       if (!disasm) return false;
       m_offsetToDisasm[offset] = disasm;
    }
    disasm = m_offsetToDisasm[offset];
    //printf("Filling table...\n");
    disasm->fillTable(settings);
    return true;
}

bool Tracer::isInternalCall(offset_t offset, Executable::addr_type inType)
{
    offset = this->convertAddr(offset, inType, Executable::RAW);
    DisasmBase* dis = getDisasmAt(offset);
    if (!dis) return NULL;

    size_t index = dis->m_disasmBuf.offsetToIndex(offset);
    if (index == INVALID_INDEX) return false;

    if (!dis->isFollowable(index) ) return false;

        mnem_type mnem = dis->getMnemTypeAtIndex(index);
    if ((mnem == MT_CALL  && !dis->isCallToRet(index)) || dis->isPushRet(index)) {
        if (dis->isImportCall(offset, inType)) return false;
        return true;
    }
    return false;
}

bool Tracer::defineFunction(offset_t offset, Executable::addr_type aType, QString name)
{
    const offset_t start = this->convertAddr(offset, aType, Executable::RAW);
    if (start == INVALID_ADDR) return false;

    this->m_funcManager.appendFunction(start, Executable::RAW);
    if (name.length() > 0) {
        this->m_nameManager.setFunctionName(start, Executable::RAW, name);
    }
    return true;
}

QString Tracer::translateBranching(DisasmBase* dis, const size_t index, FuncNameManager *nameManager) const
{
    if (!dis) return "";
    DisasmChunk *uChunk =  dis->m_disasmBuf.at(index);
    if (!uChunk) return "";

    const Executable::addr_type targetAddrType = uChunk->getTargetAddrType();
    if (targetAddrType == Executable::NOT_ADDR) {
        return uChunk->toString();
    }

    const offset_t targetOrig = dis->getTargetOffset(index, targetAddrType);
    const offset_t targetRVA = this->convertAddr(targetOrig, targetAddrType, Executable::RVA);
    const offset_t targetRaw = this->convertAddr(targetOrig, targetAddrType, Executable::RAW);
    
    if (uChunk->isTargetImm() && uChunk->isBranching()) {
        return uChunk->translateBranchingMnemonic() + " " + nameManager->getFunctionName(targetOrig, targetAddrType);
    }

    QString mnemDesc = uChunk->toString();

    if (targetRaw == INVALID_ADDR) {
        return mnemDesc;// + " <invalid>";
    }
    if (!nameManager) return mnemDesc;

    bool isNamed = nameManager->hasName(targetRVA, Executable::RVA);
    if (isNamed) {
        return mnemDesc + " " + nameManager->getFunctionName(targetRVA, Executable::RVA);
    }
    return mnemDesc;
}
