#include "CapstoneChunk.h"

using namespace minidis;

#define MAX_ARG_NUM 4

bool CapstoneChunk::fetchTargetAddr(const size_t argNum, TargetValue &targetVal, cs_detail *m_detail) const
{
    if (!m_detail) return false;

    size_t cnt = static_cast<size_t>(m_detail->x86.op_count);
    if (argNum >= cnt) return false;

    targetVal.m_targetOpNum = argNum;

    const size_t opSize = m_detail->x86.operands[argNum].size;
    const x86_op_type type = m_detail->x86.operands[argNum].type;
    const x86_reg reg = static_cast<x86_reg>(m_detail->x86.operands[argNum].mem.base);

     const bool isEIPrelative = (reg == X86_REG_IP || reg == X86_REG_EIP || reg == X86_REG_RIP);

    if (type == X86_OP_MEM) {

        int64_t lval = m_detail->x86.operands[argNum].mem.disp;

        if (reg <= X86_REG_INVALID) { //simple case, no reg value to add
            if (!isValidAddr(lval, Executable::VA)) return false;

            return setTargetVal(targetVal, argNum, Executable::VA, lval);
        }
        if (isEIPrelative) {
            const offset_t currIP = this->m_startOffset + this->getOffset() + this->getChunkSize();
            const offset_t target = currIP + lval;
            if (!isValidAddr(target, Executable::RVA)) return false;

            return setTargetVal(targetVal, argNum, Executable::RVA, target);
        }
    }

    if (type == X86_OP_IMM) {
        const bool isImm = true;
        int64_t lval = this->signExtend(m_detail->x86.operands[argNum].imm, opSize);

        //do not check registers in case of (short) branchings
        if (this->isBranching() && !isLongOp()) {
            const offset_t target = m_startOffset + lval;
            return setTargetVal(targetVal, argNum, Executable::RVA, target, isImm);
        }

        if (reg <= X86_REG_INVALID) { //simple case, no reg value to add
            if (!isValidAddr(lval, Executable::VA)) return false;

            return setTargetVal(targetVal, argNum, Executable::VA, lval, isImm);
        }
        //other registers, not supported case
        //printf("%s %s\n", this->m_insn.mnemonic, this->m_insn.op_str);
        return false;
    }
    return false;
}

TargetValue CapstoneChunk::fetchTargetAddr(cs_detail *detail)
{
    TargetValue targetVal;
    targetVal.setValues(0, Executable::NOT_ADDR, INVALID_ADDR, INVALID_ADDR);

    const size_t cnt = getMaxArg();
    if (cnt == 0) return targetVal;

    for (size_t i = 0; i < cnt; i++) {

        bool isOk = fetchTargetAddr(i, targetVal, detail);
        if (!isOk || targetVal.getAddrType() == Executable::NOT_ADDR) continue;
        // OK:
        return targetVal;
    }
    targetVal.setValues(0, Executable::NOT_ADDR, INVALID_ADDR, INVALID_ADDR);
    return targetVal;
}
//---

mnem_type CapstoneChunk::fetchMnemType(const x86_insn cMnem)
{
    if (cMnem >= X86_INS_JAE && cMnem <= X86_INS_JS) {
        if (cMnem == X86_INS_JMP || cMnem == X86_INS_LJMP) return MT_JUMP;
        return MT_COND_JUMP;
    }
    if (cMnem >= X86_INS_MOV && cMnem <= X86_INS_MOVZX) {
        return MT_MOV;
    }

    switch (cMnem) {
        case X86_INS_LOOP:
        case X86_INS_LOOPE:
        case X86_INS_LOOPNE:
            return MT_LOOP;

        case X86_INS_CALL :
        case X86_INS_LCALL:
            return MT_CALL;

        case X86_INS_RET:
        case X86_INS_RETF:
        case X86_INS_RETFQ:
            return MT_RET;

        case X86_INS_NOP : return MT_NOP;
        case X86_INS_INVALID : return MT_INVALID;

        case X86_INS_POP:
        case X86_INS_POPAW:
        case X86_INS_POPAL:
        case X86_INS_POPCNT:
        case X86_INS_POPF:
        case X86_INS_POPFD:
        case X86_INS_POPFQ:
        {
            return MT_POP;
        }
        case X86_INS_PUSH:
        case X86_INS_PUSHAW:
        case X86_INS_PUSHAL:
        case X86_INS_PUSHF:
        case X86_INS_PUSHFD:
        case X86_INS_PUSHFQ:
        {
            return MT_PUSH;
        }
        case X86_INS_INT3 :
            return MT_INT3;

        case X86_INS_INT:
            return MT_INTX;
    }
    return MT_OTHER;
}

/*
QString CapstoneChunk::valToString() const
{
    if (!m_detail) return "";

    size_t cnt = static_cast<size_t>(m_detail->x86.op_count);
    if (cnt == 0) return "";

    QString opCount = " : "+ QString::number(cnt);
    if (cnt > 0) {
        bool isOk;
        int64_t lval = this->getSignedLVal(0, isOk);
        if (!isOk) return opCount +" fail";

        QString opVal = " : "+ QString::number(lval, 16);
        return " ("+ opVal +")";
    }
    return opCount;
}*/

QString CapstoneChunk::translateBranchingMnemonic() const
{
    if (this->m_insn.mnemonic == NULL || !this->isBranching()) {
        return "";
    }

    QString desc = QString(this->m_insn.mnemonic).toUpper();

    const x86_insn cMnem = static_cast<x86_insn>(this->m_insn.id);
    if (cMnem == X86_INS_JMP) {
        desc += " SHORT";
    }
    return  desc;
}

QString CapstoneChunk::getMnemString() const
{
    if (this->m_insn.mnemonic == NULL) {
        return "";
    }
    return QString(this->m_insn.mnemonic).toUpper();
}
