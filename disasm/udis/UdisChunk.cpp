#include "UdisChunk.h"

using namespace minidis;

#define MAX_ARG_NUM 4

bool UdisChunk::fetchTargetAddr(const size_t argNum, TargetValue &targetVal) const
{
    size_t cnt = MAX_ARG_NUM;
    if (argNum >= cnt) return false;
    targetVal.m_targetOpNum = argNum;

    const ud_t &inpObj = this->ud_obj;

    bool got = false;
    offset_t lval =  getSignedLVal(inpObj, argNum, got);
    if (!got) return INVALID_ADDR;

    const ud_type opType = inpObj.operand[argNum].type;
    const ud_type regType = inpObj.operand[argNum].base;

    const offset_t currIP = this->m_startOffset + this->getOffset() + this->getChunkSize();

    if (opType == UD_OP_JIMM) {
        setTargetVal(targetVal, argNum, Executable::RVA, currIP + lval, true);
        return true;
    }
    if (opType == UD_OP_IMM) {
        setTargetVal(targetVal, argNum, Executable::VA, lval, true);
        return true;
    }

    if (opType == UD_OP_MEM) {
        if (regType == UD_NONE) { // may be VA
            setTargetVal(targetVal, argNum, Executable::VA, lval, true);
            return true;
        }

        if (regType == UD_R_RIP) {
            setTargetVal(targetVal, argNum, Executable::RVA, currIP + lval, true);
            return true;
        } 
    }
    return false;
}

TargetValue UdisChunk::fetchTargetAddr()
{
    TargetValue targetVal;
    targetVal.setValues(0, Executable::NOT_ADDR, INVALID_ADDR, INVALID_ADDR);

    const size_t cnt = getMaxArg();
    if (cnt == 0) return targetVal;

    for (size_t i = 0; i < cnt; i++) {

        bool isOk = fetchTargetAddr(i, targetVal);
        if (!isOk || targetVal.getAddrType() == Executable::NOT_ADDR) continue;
        // OK:
        return targetVal;
    }
    targetVal.setValues(0, Executable::NOT_ADDR, INVALID_ADDR, INVALID_ADDR);
    return targetVal;
}

//public:
int64_t UdisChunk::getSignedLVal(const ud_t &inpObj, size_t argNum, bool &isOk)
{
    isOk = false;
    if (argNum > MAX_ARG_NUM) return INVALID_ADDR;

    int64_t lValue = 0;
    uint8_t size = inpObj.operand[argNum].size;
    if (size == 0) return 0;

    uint8_t maxSize = sizeof(uint64_t) * 8; // in bits
    uint8_t dif = maxSize - size;

    lValue = inpObj.operand[argNum].lval.uqword;
    int64_t mlValue = (lValue << dif) >> dif; // gives signed!!!
    isOk = true;
    return mlValue;
}

//---

mnem_type UdisChunk::fetchMnemType() const
{
    switch(ud_obj.mnemonic)
    {
        case UD_Ijo : case UD_Ijno : case UD_Ijb : case UD_Ijae :
        case UD_Ijz : case UD_Ijnz : case UD_Ijbe : case UD_Ija :
        case UD_Ijs : case UD_Ijns : case UD_Ijp : case UD_Ijnp :
        case UD_Ijl : case UD_Ijge : case UD_Ijle : case UD_Ijg :
        case UD_Ijcxz : case UD_Ijecxz : case UD_Ijrcxz :
            return MT_COND_JUMP;

        case UD_Ijmp :
            return MT_JUMP;

        case UD_Iloop: return MT_LOOP;
        case UD_Icall : return MT_CALL;

        case UD_Iret:
        case UD_Iretf :
            return MT_RET;

        case UD_Inop : return MT_NOP;
        case UD_Iinvalid :
            return MT_INVALID;

        case UD_Ipush :
        case UD_Ipusha:
        case UD_Ipushad:
        case UD_Ipushfd:
        case UD_Ipushfq:
        case UD_Ipushfw:
            return MT_PUSH;

        case UD_Ipop :
        case UD_Ipopa:
        case UD_Ipopad:
        case UD_Ipopcnt:
        case UD_Ipopfd:
        case UD_Ipopfq:
        case UD_Ipopfw:
            return MT_POP;

        case UD_Iint3 :
            return MT_INT3;

        case UD_Iint:
            return MT_INTX;
    }
    return MT_OTHER;
}

QString UdisChunk::translateBranchingMnemonic() const
{
    ud_mnemonic_code_t mnem = ud_obj.mnemonic;
    QString mnemDesc = "";
    switch (mnem) {
        case UD_Iloop: mnemDesc = "LOOP"; break;
        case UD_Ijo : mnemDesc = "JO"; break;
        case UD_Ijno : mnemDesc = "JNO"; break;
        case UD_Ijb : mnemDesc = "JB"; break;
        case UD_Ijae : mnemDesc = "JLAE"; break;
        case UD_Ijz : mnemDesc = "JZ"; break;
        case UD_Ijnz : mnemDesc = "JNZ"; break;
        case UD_Ijbe : mnemDesc = "JBE"; break;
        case UD_Ija : mnemDesc = "JA"; break;
        case UD_Ijs : mnemDesc = "JS"; break;
        case UD_Ijns : mnemDesc = "JNS"; break;
        case UD_Ijp : mnemDesc = "JP"; break;
        case UD_Ijnp : mnemDesc = "JNP"; break;
        case UD_Ijl : mnemDesc = "JL"; break;
        case UD_Ijge : mnemDesc = "JGE"; break;
        case UD_Ijle : mnemDesc = "JLE"; break;
        case UD_Ijg : mnemDesc = "JG"; break;
        case UD_Ijcxz : mnemDesc = "JCXZ"; break;
        case UD_Ijecxz : mnemDesc = "JECXZ"; break;
        case UD_Ijrcxz : mnemDesc = "JRCXZ"; break;
        case UD_Ijmp : mnemDesc = "JMP"; break;
        case UD_Icall : mnemDesc = "CALL"; break;
    }
    return mnemDesc;
}

bool UdisChunk::isAddrOperand() const
{
    if (ud_obj.mnemonic == UD_Ipush || ud_obj.mnemonic == UD_Imov) return true;

    for (int i = 0 ; i < MAX_ARG_NUM; i++) {
        if (ud_obj.operand[i].type == UD_OP_IMM &&
            ud_obj.operand[i].size > 8)
        {
            return true;
        }
    }
    return false;
}

