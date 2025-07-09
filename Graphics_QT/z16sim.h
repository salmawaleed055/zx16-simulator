#ifndef Z16SIM_H
#define Z16SIM_H

#include <QMainWindow>
#include <QImage>
#include <QColor>
#include <vector>
#include <cstdint>
#include <cstddef>

class z16sim : public QMainWindow
{
    Q_OBJECT

public:
    explicit z16sim(QWidget *parent = nullptr);  // 1st constructor
    z16sim(bool debugMode);                      // 2nd constructor
    ~z16sim();
   // explicit z16sim(QWidget *parent = nullptr);
    //~z16sim();

    static constexpr int NUM_REGS = 8; // Declare first!
    static const size_t MEM_SIZE = 65536;
    static const int RA_REG = 1;
    QImage renderTiledScreen();
    void dumpRegisters() const;
    void loadMemoryFromFile(const char* filename);
    bool cycle();
    int executeInstruction(uint16_t inst);
    void reset();

    uint16_t getPC() const { return pc; }
    void setPC(uint16_t new_pc) { this->pc = new_pc; }
    void setDebug(bool d);
    bool isDebug() const;
    void setVerbose(bool val);
    bool isVerbose() const;

    void disassemble(uint16_t inst, uint16_t current_pc, char* buf, size_t bufSize);

    uint16_t getReg(int index) const {
        if (index >= 0 && index < NUM_REGS) return regs[index];
        return 0;
    }
    unsigned char getMemByte(uint16_t addr) const {
        if (addr < MEM_SIZE) return memory[addr];
        return 0;
    }
   unsigned char memory[MEM_SIZE];
//    QImage renderTiledScreen(const std::vector<quint8>& memory);
private:
    //Ui::z16sim *ui;

    uint16_t regs[NUM_REGS];
    uint16_t pc;

    bool debug;
    bool verbose;
    int infinityCheck[4];
    //z16sim();
    static const char* regNames[NUM_REGS]; // Now NUM_REGS is known here
    void setReg(uint8_t reg_idx, uint16_t value);
};

#endif // Z16SIM_H
