#ifndef EVER2E_CPU65C02_OPCODE_H
#define EVER2E_CPU65C02_OPCODE_H

#include <cstdint>
#include <vector>

class Cpu65c02Opcode {
public:
    enum class MicroOp : uint8_t {
        M_FETCH_OPCODE,
        M_FETCH_OPERAND_LO,
        M_FETCH_OPERAND_HI,
        M_READ_IMM_DATA,
        M_READ_ZP_PTR_LO,
        M_READ_ZP_PTR_HI,
        M_READ_DUMMY,
        M_READ_EA,
        M_WRITE_EA_DUMMY,
        M_WRITE_EA,
        M_INTERNAL
    };

    enum class AccessType : uint8_t { AT_NONE, AT_READ, AT_WRITE, AT_RMW };

    struct MicroCycleProgram {
        AccessType accessType;
        std::vector<MicroOp> noCrossScript;
        std::vector<MicroOp> crossScript;
    };

    struct Definition {
        const char* name;
        uint8_t opcodeByte;
        MicroCycleProgram microcode;
    };

    enum class Id : uint16_t {
        LDA_IMM,
        LDA_ZPG,
        LDA_ZPG_X,
        LDA_ABS,
        LDA_ABS_X,
        LDA_ABS_Y,
        LDA_IND_X,
        LDA_IND_Y,
        LDA_IND,
        STA_ZPG,
        STA_ZPG_X,
        STA_ABS,
        STA_ABS_X,
        STA_ABS_Y,
        STA_IND_X,
        STA_IND_Y,
        STA_IND,
        INC_ZPG,
        INC_ZPG_X,
        INC_ABS,
        INC_ABS_X,
        DEC_ZPG,
        DEC_ZPG_X,
        DEC_ABS,
        DEC_ABS_X,
        ASL_ACC,
        ASL_ZPG,
        ASL_ZPG_X,
        ASL_ABS,
        ASL_ABS_X,
        LSR_ACC,
        LSR_ZPG,
        LSR_ZPG_X,
        LSR_ABS,
        LSR_ABS_X,
        ROL_ACC,
        ROL_ZPG,
        ROL_ZPG_X,
        ROL_ABS,
        ROL_ABS_X,
        ROR_ACC,
        ROR_ZPG,
        ROR_ZPG_X,
        ROR_ABS,
        ROR_ABS_X,
        ORA_IMM,
        ORA_ZPG,
        ORA_ZPG_X,
        ORA_ABS,
        ORA_ABS_X,
        ORA_ABS_Y,
        ORA_IND_X,
        ORA_IND_Y,
        ORA_IND,
        AND_IMM,
        AND_ZPG,
        AND_ZPG_X,
        AND_ABS,
        AND_ABS_X,
        AND_ABS_Y,
        AND_IND_X,
        AND_IND_Y,
        AND_IND,
        EOR_IMM,
        EOR_ZPG,
        EOR_ZPG_X,
        EOR_ABS,
        EOR_ABS_X,
        EOR_ABS_Y,
        EOR_IND_X,
        EOR_IND_Y,
        EOR_IND,
        ADC_IMM,
        ADC_ZPG,
        ADC_ZPG_X,
        ADC_ABS,
        ADC_ABS_X,
        ADC_ABS_Y,
        ADC_IND_X,
        ADC_IND_Y,
        ADC_IND,
        SBC_IMM,
        SBC_ZPG,
        SBC_ZPG_X,
        SBC_ABS,
        SBC_ABS_X,
        SBC_ABS_Y,
        SBC_IND_X,
        SBC_IND_Y,
        SBC_IND,
        CMP_IMM,
        CMP_ZPG,
        CMP_ZPG_X,
        CMP_ABS,
        CMP_ABS_X,
        CMP_ABS_Y,
        CMP_IND_X,
        CMP_IND_Y,
        CMP_IND,
        BIT_IMM,
        BIT_ZPG,
        BIT_ZPG_X,
        BIT_ABS,
        BIT_ABS_X,
        LDX_IMM,
        LDX_ZPG,
        LDX_ZPG_Y,
        LDX_ABS,
        LDX_ABS_Y,
        LDY_IMM,
        LDY_ZPG,
        LDY_ZPG_X,
        LDY_ABS,
        LDY_ABS_X,
        STX_ZPG,
        STX_ZPG_Y,
        STX_ABS,
        STY_ZPG,
        STY_ZPG_X,
        STY_ABS,
        CPX_IMM,
        CPX_ZPG,
        CPX_ABS,
        CPY_IMM,
        CPY_ZPG,
        CPY_ABS,
        JSR_ABS,
        BRK_IMP,
        TSB_ZPG,
        PHP_IMP,
        TSB_ABS,
        BPL_REL,
        TRB_ZPG,
        CLC_IMP,
        INA_ACC,
        TRB_ABS,
        PLP_IMP,
        BMI_REL,
        SEC_IMP,
        DEA_ACC,
        RTI_IMP,
        PHA_IMP,
        JMP_ABS,
        BVC_REL,
        CLI_IMP,
        PHY_IMP,
        RTS_IMP,
        STZ_ZPG,
        PLA_IMP,
        JMP_ABS_IND,
        BVS_REL,
        STZ_ZPG_X,
        SEI_IMP,
        PLY_IMP,
        JMP_ABS_IND_X,
        BRA_REL,
        DEY_IMP,
        TXA_IMP,
        BCC_REL,
        TYA_IMP,
        TXS_IMP,
        STZ_ABS,
        STZ_ABS_X,
        TAY_IMP,
        TAX_IMP,
        BCS_REL,
        CLV_IMP,
        TSX_IMP,
        INY_IMP,
        DEX_IMP,
        BNE_REL,
        CLD_IMP,
        PHX_IMP,
        INX_IMP,
        BEQ_REL,
        SED_IMP,
        PLX_IMP
    };

    static const Definition& byId(Id id);
    static const Definition* fromOpcodeByte(uint8_t opcodeByte);
    static const std::vector<Id>& values();

    static const std::vector<Id>& adcFamily();
    static std::vector<uint8_t> adcOpcodeBytes();
    static const std::vector<Id>& andFamily();
    static std::vector<uint8_t> andOpcodeBytes();
    static const std::vector<Id>& aslFamily();
    static std::vector<uint8_t> aslOpcodeBytes();
    static const std::vector<Id>& bitFamily();
    static std::vector<uint8_t> bitOpcodeBytes();
    static const std::vector<Id>& bitTestSetFamily();
    static std::vector<uint8_t> bitTestSetOpcodeBytes();
    static const std::vector<Id>& branchFamily();
    static std::vector<uint8_t> branchOpcodeBytes();
    static const std::vector<Id>& cmpFamily();
    static std::vector<uint8_t> cmpOpcodeBytes();
    static const std::vector<Id>& controlMiscFamily();
    static std::vector<uint8_t> controlMiscOpcodeBytes();
    static const std::vector<Id>& cpxFamily();
    static std::vector<uint8_t> cpxOpcodeBytes();
    static const std::vector<Id>& cpyFamily();
    static std::vector<uint8_t> cpyOpcodeBytes();
    static const std::vector<Id>& decFamily();
    static std::vector<uint8_t> decOpcodeBytes();
    static const std::vector<Id>& eorFamily();
    static std::vector<uint8_t> eorOpcodeBytes();
    static const std::vector<Id>& flagsFamily();
    static std::vector<uint8_t> flagsOpcodeBytes();
    static const std::vector<Id>& incFamily();
    static std::vector<uint8_t> incOpcodeBytes();
    static const std::vector<Id>& indexIncDecFamily();
    static std::vector<uint8_t> indexIncDecOpcodeBytes();
    static const std::vector<Id>& interruptControlFlowFamily();
    static std::vector<uint8_t> interruptControlFlowOpcodeBytes();
    static const std::vector<Id>& jsrFamily();
    static std::vector<uint8_t> jsrOpcodeBytes();
    static const std::vector<Id>& jumpFamily();
    static std::vector<uint8_t> jumpOpcodeBytes();
    static const std::vector<Id>& ldaFamily();
    static std::vector<uint8_t> ldaOpcodeBytes();
    static const std::vector<Id>& ldxFamily();
    static std::vector<uint8_t> ldxOpcodeBytes();
    static const std::vector<Id>& ldyFamily();
    static std::vector<uint8_t> ldyOpcodeBytes();
    static const std::vector<Id>& logicalArithmeticFamily();
    static std::vector<uint8_t> logicalArithmeticOpcodeBytes();
    static const std::vector<Id>& lsrFamily();
    static std::vector<uint8_t> lsrOpcodeBytes();
    static const std::vector<Id>& oraFamily();
    static std::vector<uint8_t> oraOpcodeBytes();
    static const std::vector<Id>& readModifyWriteFamily();
    static std::vector<uint8_t> readModifyWriteOpcodeBytes();
    static const std::vector<Id>& rolFamily();
    static std::vector<uint8_t> rolOpcodeBytes();
    static const std::vector<Id>& rorFamily();
    static std::vector<uint8_t> rorOpcodeBytes();
    static const std::vector<Id>& sbcFamily();
    static std::vector<uint8_t> sbcOpcodeBytes();
    static const std::vector<Id>& staFamily();
    static std::vector<uint8_t> staOpcodeBytes();
    static const std::vector<Id>& stackFamily();
    static std::vector<uint8_t> stackOpcodeBytes();
    static const std::vector<Id>& stxFamily();
    static std::vector<uint8_t> stxOpcodeBytes();
    static const std::vector<Id>& styFamily();
    static std::vector<uint8_t> styOpcodeBytes();
    static const std::vector<Id>& stzFamily();
    static std::vector<uint8_t> stzOpcodeBytes();
    static const std::vector<Id>& transferFamily();
    static std::vector<uint8_t> transferOpcodeBytes();
};

#endif
