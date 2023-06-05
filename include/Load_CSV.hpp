#ifndef __LOAD_CSV_HPP__
#define __LOAD_CSV_HPP__

#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <torch/torch.h>
#include <string> 
#include <utility>


torch::Tensor load_csv(std::string filename)
    size_t i =0;

    std::fstream infile(filename);
    char buffer[65536];
    infile.rdbuf()->pubsetbuf(buffer, sizeof(buffer));
    std::string line;
    std::vector<std::string> splittedString;
      
    bool condition = 0 ;
        
    while (getline(infile, line)) {
            
        for(auto & t : row_toRemove){
            if(i == t)
                condition = 1;
       }
        if(condition == 1){
            splittedString.clear();
        }
        else{ 
            splittedString.clear();
            size_t last = 0, pos = 0;
                
            while ((pos = line.find(',', last)) != std::string::npos) {
                splittedString.emplace_back(line, last, pos - last);
                last = pos + 1;
            }
            if (last)
                splittedString.emplace_back(line, last);

            total_cols = splittedString.size() - total_rm_cols;
            if(col_toRemove.size() > 0 )
                m_remove(splittedString, col_toRemove);

            for(auto & elm : splittedString){
                tensor_vec.push_back(std::stof(elm));
            }
            //std::cout << tensor_vec[0] << std::endl;
        }
        i++;
    }

    int total_row = i - total_rm_row;
    torch::Tensor myTensor = torch::from_blob(tensor_vec.data(), {total_row, total_cols});  
    return myTensor.clone();
}


#endif // __LOAD_CSV_HPP__