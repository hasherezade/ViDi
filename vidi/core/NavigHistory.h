#pragma once
#include <QtCore>
#include <bearparser.h>

class NavigHistory : public QObject
{
Q_OBJECT

signals:
    //void currentOffsetChanged(offset_t);
    void stateChanged();

public:

    NavigHistory() {}
    virtual ~NavigHistory() {}

    void append(offset_t offset)
    {
        //printf("Appenging to undo: %llx\n", offset);
        m_undoStack.push(offset);
        m_redoStack.clear();
    }

    bool canUndo() {
        if (m_undoStack.size() <= 1) {
            return false;
        }
        return true;
    }

    bool canRedo() {
        if (m_redoStack.size() == 0) {
            return false;
        }
        return true;
    }

    offset_t undo()
    {
        if (!canUndo()) {
            return INVALID_ADDR;
        }
        offset_t offset = m_undoStack.top(); // current
        m_undoStack.pop();
        m_redoStack.push(offset);

        offset = m_undoStack.top(); // previous
        //printf("Undo to: %llx\n", offset);

        emit stateChanged();
        return offset;
    }

    offset_t redo()
    {
        if (!canRedo()) {
            return INVALID_ADDR;
        }

        offset_t offset = m_redoStack.top();
        m_redoStack.pop();
        m_undoStack.push(offset);

        //printf("Redo to: %llx\n", offset);
        emit stateChanged();
        return offset;
    }

protected:
    QStack<offset_t> m_undoStack;
    QStack<offset_t> m_redoStack;

friend class ExeHandler;
};
