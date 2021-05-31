#include "FileLoader.hpp"


FileLoader::FileLoader():
    files()
{
    std::ios_base::sync_with_stdio(false); 
    //files.reserve(180);
    filesOpened.reserve(180);
    std::string zero = "../hgt_files/zero.hgt";
    if(!std::filesystem::exists(zero)){
        std::ofstream ofs(zero, std::ios::binary);
        int16_t zero_num = 0;
        for(int i = 0; i < 1201*1201; i++){
            ofs.write(reinterpret_cast<char *>(&zero_num), sizeof(zero_num));
        }
        ofs.close();
    }
    FILE * zero_ptr = fopen(zero.c_str(), "rb");

    for(int i = 0; i < 180; i++){
        //files.emplace_back(std::vector<std::string>());
        //files.back().reserve(360);
        filesOpened.emplace_back(std::vector<FILE *>());
        filesOpened.back().reserve(360);
        for(int j = 0; j < 360; j++){
            std::string filename = "../hgt_files/";
            std::string NS = std::to_string(std::abs(i-90));
            if(i-90 >= 0){
                filename.append("N")
                    .append(std::string(2 - NS.length(), '0'))
                    .append(NS);
            }
            else
            {
                filename.append("S")
                    .append(std::string(2 - NS.length(), '0'))
                    .append(NS);
            }

            std::string EW = std::to_string(std::abs(j-180));
            if(j-180 >= 0){
                filename.append("E")
                    .append(std::string(3 - EW.length(), '0'))
                    .append(EW);
            }
            else
            {
                filename.append("W")
                    .append(std::string(3 - EW.length(), '0'))
                    .append(EW);
            }
            filename.append(".hgt");
            if(std::filesystem::exists(filename)){
                //std::cout << i << " " << j << " " << i*j << "\n";
                //this->files[i].emplace_back(filename);
                this->filesOpened[i].emplace_back(fopen(filename.c_str(), "rb"));

            }
            else{
                //this->files[i].emplace_back(zero);
                this->filesOpened[i].emplace_back(zero_ptr);
            }
            //files[i].back()->exceptions(std::ifstream::failbit | std::ifstream::badbit);
        }
    }
    std::cout << "Reading data done\n";
}

std::ifstream FileLoader::getFile(int verDeg, int horDeg){
    return std::ifstream(files[verDeg][horDeg], std::ios::binary);
}

FILE * FileLoader::getFilePointer(int verDeg, int horDeg){
    return filesOpened[verDeg][horDeg];
}

void FileLoader::getHeight(int16_t& n, size_t verDeg, size_t horDeg, size_t verI, size_t horI){
    FILE * file = getFilePointer(verDeg,horDeg);
    if(file == NULL ) perror("error");
    fseek(file, ((verI + horI)%(1201*1201)) * sizeof(int16_t), SEEK_SET);
    fread(&n, sizeof(int16_t), 1, file);
    //getFile(verDeg,horDeg)
    //    .seekg(((verI + horI)%(1201*1201))*sizeof(int16_t), std::ios::beg)
    //    .read(reinterpret_cast<char *>(&n), sizeof(n));
    n = __builtin_bswap16(n);
}



FileLoader::~FileLoader(){
    for(int i = 0; i < 180; i++){
        for(int j = 0; j < 360; j++){
            fclose(filesOpened[i][j]);
        }
    }
    std::ios_base::sync_with_stdio(true); 
}