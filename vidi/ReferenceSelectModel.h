#pragma once

#include <QAbstractTableModel>
#include "core/Executables.h"

class ReferenceSelectModel : public QObject
{
    Q_OBJECT

signals:
    void selectedReferencesChanged();

public slots:
    void onSelectedReferences(const offset_t refOffset, const  Executable::addr_type aType)
    {
        if (setSelectedReferences(refOffset, aType)) {
            emit selectedReferencesChanged();
        }
    }

    virtual void setExecutable(ExeHandler* exeHndl)
    {
        this->m_Tracer = NULL;
        this->m_ExeHandler = exeHndl;

        if (this->m_ExeHandler) {
            m_Tracer  = m_ExeHandler->getTracer();
        }
        onSelectedReferences(INVALID_ADDR, Executable::NOT_ADDR);
    }

public:
    ReferenceSelectModel(QObject *v_parent)
        : QObject(v_parent), m_ExeHandler(NULL), m_Tracer(NULL),
        m_refOffset(INVALID_ADDR), m_refsType(Executable::NOT_ADDR)
    {
    }
    
    const QString &getNameOfRefered() const
    {
        return m_nameOfRefered;
    }

    offset_t getRefOffset() const
    {
        return this->m_refOffset;
    }

    const Executable::addr_type getRefsType() const
    {
        return this->m_refsType;
    }
    
    bool isOffsetRefered(const offset_t offset, const Executable::addr_type aType) const
    {
        if (offset == INVALID_ADDR || m_refOffset == INVALID_ADDR) return false;

        const offset_t refConverted = m_Tracer->convertAddr(m_refOffset, m_refsType, aType);
        return (offset == refConverted);
    }

    bool isBlockContainingRef(CodeBlock *block) const
    {
        if (!m_Tracer || !block) return false;

        const Executable::addr_type blockAddrType = block->getAddrType();
        offset_t refRaw= m_Tracer->convertAddr(m_refOffset, m_refsType, blockAddrType);
        if (block->contains(refRaw)) {
            return true;
        }
        return false;
    }

    bool isBlockRefered(CodeBlock *block) const
    {
        if (!m_Tracer || !block) return false;
        if (m_refOffset == INVALID_ADDR) return false;

        QSet<CodeBlock*>* bSet = m_Tracer->refBlocksTo(m_refOffset, m_refsType);
        if (bSet && bSet->contains(block)) {
            return true;
        }
        return false;
    }

protected:
    bool setSelectedReferences(const offset_t refOffset, const Executable::addr_type aType)
    {
        if ( m_refOffset == refOffset && m_refsType == aType) return false;

        m_refOffset = refOffset;
        m_refsType = aType;
        m_nameOfRefered = this->makeNameOfRefered();
        return true;
    }

    QString makeNameOfRefered() const
    {
        if (!m_Tracer) return "";
        if (m_refOffset == INVALID_ADDR) return "";

        if (m_Tracer->isImportedFunction(m_refOffset, m_refsType)) {
            return m_Tracer->getImportName(m_refOffset, m_refsType);
        }
        if (m_Tracer->hasName(m_refOffset, m_refsType)){
            return m_Tracer->getFunctionName(m_refOffset, m_refsType);
        }
        if (m_Tracer->hasReferedString(m_refOffset, m_refsType)) {
             return m_Tracer->getReferedString(m_refOffset, m_refsType);
        }
        return "";//
    }

    ExeHandler *m_ExeHandler;
    Tracer* m_Tracer;
    offset_t m_refOffset;
    Executable::addr_type m_refsType;
    QString m_nameOfRefered;
};
