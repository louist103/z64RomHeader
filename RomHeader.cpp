#include <iostream>
#include <fstream>
#include <endian.h>
#include <iomanip>
#include <cstring>

#define SPACING "             "

typedef struct RomHeader {
    char domain1Reg[4];
    unsigned int clockRate;
    unsigned int entryPoint;
    unsigned int revision;
    unsigned int checkSum1;
    unsigned int checkSum2;
    unsigned int unknown1;
    unsigned int unknown2;
    char name[20];
    unsigned int unknown3;
    unsigned int cart;
    char cartId[2];
    char region;
    char version;
} RomHeader;

int main(int argc, char** argv) {
    RomHeader romHeader;
    std::ifstream inFile;
    std::ofstream outFile;
    
    if(argc < 2){
        std::cout<<"Not enough arguments passed.  Must pass at least the relative path of the baserom\n";
        return -1;
    } else if ((argc >= 2) && ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1],"-help") == 0))) {
        std::cout << "Rom header extraction tool.  Example use ./RomHeader \"../baserom.z64\"\nOptionally use \"../baserom.z64\" P to force region flag \"P\"\n";
        return 0;
    }

    inFile.open(argv[1], std::ios::binary | std::ios::in);

    if(!inFile.is_open()){
        std::cout<<"Error: Failed to open " << "baserom.z64\n";
        return -1;
    }

    inFile.read(romHeader.domain1Reg,sizeof(romHeader.domain1Reg));

    inFile.read((char*)&romHeader.clockRate, sizeof(romHeader.clockRate));
    romHeader.clockRate = htobe32(romHeader.clockRate);

    inFile.read((char*)&romHeader.entryPoint, sizeof(romHeader.entryPoint));
    romHeader.entryPoint = htobe32(romHeader.entryPoint);

    inFile.read((char*)&romHeader.revision, sizeof(romHeader.revision));
    romHeader.revision = htobe32(romHeader.revision);

    inFile.read((char*)&romHeader.checkSum1, sizeof(romHeader.checkSum1));
    romHeader.checkSum1 = htobe32(romHeader.checkSum1);

    inFile.read((char*)&romHeader.checkSum2, sizeof(romHeader.checkSum2));
    romHeader.checkSum2 = htobe32(romHeader.checkSum2);

    inFile.read((char*)&romHeader.unknown1, sizeof(romHeader.unknown1));
    romHeader.unknown1 = htobe32(romHeader.unknown1);
    
    inFile.read((char*)&romHeader.unknown2, sizeof(romHeader.unknown2));
    romHeader.unknown2 = htobe32(romHeader.unknown2);

    inFile.read(romHeader.name, sizeof(romHeader.name));

    inFile.read((char*)&romHeader.unknown3, sizeof(romHeader.unknown3));
    romHeader.unknown3 = htobe32(romHeader.unknown3);

    inFile.read((char*)&romHeader.cart, sizeof(romHeader.cart));
    romHeader.cart = htobe32(romHeader.cart);

    inFile.read(romHeader.cartId, sizeof(romHeader.cartId));
    
    inFile.read(&romHeader.region, sizeof(romHeader.region));
    
    inFile.read(&romHeader.version, sizeof(romHeader.version));

    inFile.close();

    outFile.open("rom_header.s",std::ios::out);
    if(!outFile.is_open()){
        std::cout<<"Error: Failed to open " << "rom_header.s\n";
        return -1;
    }

    outFile << "/*\n* The Legend of Zelda: Ocarina of Time ";
    switch (romHeader.checkSum1){
        case 0x917D18F6:
            outFile << "Master Quest Debug " << "ROM Header\n/*\n\n";
            break;
        case 0x87121EFE:
            outFile << "GameCube Debug (03-13-2002 build) " << "ROM Header\n/*\n\n";
            break; 
        case 0x871E1C92:
            outFile << "GameCube Debug (03-21-2002 build) " << "ROM Header\n/*\n\n";

    }
    outFile << std::uppercase << std::hex;
    outFile << ".byte " << "0x" << (((int)romHeader.domain1Reg[0]) & 0xFF) << ", 0x" << (int)romHeader.domain1Reg[1] << ", 0x" << (int)romHeader.domain1Reg[2] << ", 0x" << (int)romHeader.domain1Reg[3] << " /* PI BSD Domain 1 register */\n";
    outFile << ".word " << "0x" << std::setfill('0') << std::setw(8) << romHeader.clockRate << SPACING <<"/* Clockrate setting */\n";
    outFile << ".word " << "0x" << std::setfill('0') << std::setw(8) << romHeader.entryPoint << SPACING << "/* Entrypoint function (`entrypoint`) */\n";
    outFile << ".word " << "0x" << std::setfill('0') << std::setw(8) << romHeader.revision << SPACING << "/* Revision */\n";
    outFile << ".word " << "0x" << std::setfill('0') << std::setw(8) << romHeader.checkSum1 << SPACING << "/* Checksum1 */\n";
    outFile << ".word " << "0x" << std::setfill('0') << std::setw(8) << romHeader.checkSum2 << SPACING << "/* Checksum2 */\n";
    outFile << ".word " << "0x" << std::setfill('0') << std::setw(8) << romHeader.unknown1 << SPACING << "/* Unknown */\n";
    outFile << ".word " << "0x" << std::setfill('0') << std::setw(8) << romHeader.unknown2 << SPACING << "/* Unknown */\n";
    outFile << ".ascii " << "\"" << romHeader.name << "\"" << "/* Internal ROM name */\n";
    outFile << ".word " << "0x" << std::setfill('0') << std::setw(8) << romHeader.unknown3 << SPACING << "/* Unknown */\n";
    outFile << ".word " << "0x" << std::setfill('0') << std::setw(8) << romHeader.cart << SPACING << "/* Cartridge */\n";
    outFile << ".ascii " << "\"" << romHeader.cartId[0] << romHeader.cartId[1] << "\"" << SPACING << "     /* Cartridge ID */\n";

    if((argc > 2) && (toupper(argv[2][0]) == 'P' || toupper(argv[2][0]) == 'E' || toupper(argv[2][0] == 'J'))){
        std::cout << "Forcing region flag " << argv[2] << '\n';
        outFile << ".ascii " << "\"" << toupper(argv[2][0]) << "\"" << SPACING << "      /* Region */\n";    
    } else {
        outFile << ".ascii " << "\"" << romHeader.region << "\"" << SPACING << "      /* Region */\n";
    }

    outFile << ".byte " <<  "0x" << std::setfill('0') << std::setw(2) << (((int)romHeader.version) & 0xFF) << SPACING << "      /* Version */\n";
    outFile.close();


}