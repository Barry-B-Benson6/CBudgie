#include "Translator.h"

Translator::Translator(std::filesystem::path newFilePath)
    : filePath(newFilePath) {
  std::vector<std::vector<Token>> tokenized_code;
  std::vector<std::string> bytecode;
  Lexer lexer(filePath, tokenized_code, lexerMutex_, lexerCv_, lexingDone);

  std::thread lexerThread(&Lexer::run, &lexer);
  std::thread translatorThread(&Translator::translate, this, std::ref(tokenized_code), std::ref(bytecode));

  lexerThread.join();
  translatorThread.join();
  
  writeToFile(bytecode, filePath.replace_extension(".bbg"));
}

void Translator::translate(std::vector<std::vector<Token>>& tokenized_code, std::vector<std::string>& bytecode) {
    std::unordered_map<std::string,int> variableMapByString;
    std::unordered_map<int,std::string> variableMapByInt;
    std::unordered_map<std::string,int> functionMap;
    std::unordered_map<std::string,std::pair<int,int>> undefinedFunctionMap;  
    int translationCount = 0;
    std::stack<int> scopeCounts;
    std::stack<std::pair<int,int>> tempIfJump;
    std::stack<std::pair<int,int>> loopPointer;
    std::vector<Token>::iterator linker;
  
  while (true) {
    std::unique_lock<std::mutex> lock(lexerMutex_);
    lexerCv_.wait(lock, [&tokenized_code, this]() { return !tokenized_code.empty() || lexingDone; });
    if (!tokenized_code.empty()) {
      std::vector<Token> tokens = tokenized_code.front();
      if (!tokenized_code.empty()) {
        tokenized_code.erase(tokenized_code.begin());
      }
      for (auto it = tokens.begin(); it < tokens.end(); ++it) {
        switch (it->tokenType) {
          case TokenType::LINKER:
            linker = it-1;
            break;
          case TokenType::TO_NUM:
            if (linker->tokenType == TokenType::VARIABLIE) {
                auto variable = variableMapByString.find(linker->code);
                if ((linker-1)->tokenType == TokenType::EQUAL)
                {
                    bytecode.push_back(OpcodeToString.find(Opcode::LOAD)->second + " " +
                               std::to_string(variable->second) + " " + "0");
                    
                }
                bytecode.push_back(OpcodeToString.find(Opcode::TO_NUM)->second);
                bytecode.push_back(OpcodeToString.find(Opcode::STORE)->second +
                                    " " + std::to_string(variable->second) + " " +
                                    "0");
                
            } else throw std::runtime_error("Invaild usagr of to Num !!" );
            break;
           case TokenType::TO_STRING:
            if (linker->tokenType == TokenType::VARIABLIE) {
                auto variable = variableMapByString.find(linker->code);
                if ((linker-1)->tokenType == TokenType::EQUAL)
                {
                    bytecode.push_back(OpcodeToString.find(Opcode::LOAD)->second + " " +
                               std::to_string(variable->second) + " " + "0");
                    
                }
                bytecode.push_back(OpcodeToString.find(Opcode::TO_STRING)->second);
                bytecode.push_back(OpcodeToString.find(Opcode::STORE)->second +
                                    " " + std::to_string(variable->second) + " " +
                                    "0");
                
            } else throw std::runtime_error("Invaild usagr of toString !!" );
            break;
          case TokenType::TO_BOOL:
            if (linker->tokenType == TokenType::VARIABLIE) {
                auto variable = variableMapByString.find(linker->code);
                if ((linker-1)->tokenType == TokenType::EQUAL)
                {
                    bytecode.push_back(OpcodeToString.find(Opcode::TO_BOOL)->second + " " +
                               std::to_string(variable->second) + " " + "0");
                    
                }
                bytecode.push_back(OpcodeToString.find(Opcode::TO_NUM)->second);
                bytecode.push_back(OpcodeToString.find(Opcode::STORE)->second +
                                    " " + std::to_string(variable->second) + " " +
                                    "0");
                
            } else throw std::runtime_error("Invaild usagr of toBool !!" );
            break;  
          case TokenType::WHILE:
            bytecode.push_back(OpcodeToString.find(Opcode::NOT)->second);
            translationCount=bytecode.size()-1;
            loopPointer.push(std::make_pair<int,int>((int)scopeCounts.size(),(int)translationCount-std::distance(tokens.begin(), it)));
            bytecode.push_back(OpcodeToString.find(Opcode::IF_JUMP)->second + " ");
            tempIfJump.push(std::make_pair<int,int>((int)scopeCounts.size(),(int)translationCount+1));
            break;
          case TokenType::BREAK:
            bytecode.push_back(OpcodeToString.find(Opcode::ENDSCOPE)->second);
            if (scopeCounts.top() != variableMapByString.size())
            {
                for (int i = scopeCounts.top(); i < variableMapByInt.size(); i++)
                {
                    variableMapByString.erase(variableMapByInt.find(i)->second);
                    variableMapByInt.erase(i);
                } 
            }
            bytecode.push_back(OpcodeToString.find(Opcode::JUMP)->second + " ");
            tempIfJump.push(std::make_pair<int,int>((int)tempIfJump.top().first,(int)translationCount));
            break;  
          case TokenType::ELSE:
            bytecode.push_back(OpcodeToString.find(Opcode::IF_JUMP)->second + " ");
            translationCount=bytecode.size()-1;
            tempIfJump.push(std::make_pair<int,int>((int)scopeCounts.size(),(int)translationCount));
            break;
          case TokenType::IF:
            bytecode.push_back(OpcodeToString.find(Opcode::NOT)->second);
            bytecode.push_back(OpcodeToString.find(Opcode::IF_JUMP)->second + " ");
            translationCount=bytecode.size()-1;
            tempIfJump.push(std::make_pair<int,int>((int)scopeCounts.size(),(int)translationCount));
            break;
          case TokenType::CONTINUE:
            bytecode.push_back(OpcodeToString.find(Opcode::ENDSCOPE)->second);
            if (scopeCounts.top() != variableMapByString.size())
            {
                for (int i = scopeCounts.top(); i < variableMapByInt.size(); i++)
                {
                    variableMapByString.erase(variableMapByInt.find(i)->second);
                    variableMapByInt.erase(i);
                } 
            }
            bytecode.push_back(OpcodeToString.find(Opcode::JUMP)->second + " " + std::to_string(loopPointer.top().second));
            break;
          case TokenType::DEF:
                {
                 while (it != tokens.end() && it->tokenType != TokenType::FUNCTION)
                 {
                    it++;
                 }
                if (it == tokens.end()) throw std::invalid_argument("Missing function name!!!");
                auto function = functionMap.find(it->code);
                translationCount=bytecode.size()-1;
                if (function == functionMap.end()) functionMap.emplace(it->code, translationCount+2);
                else if (function->second == -1) {
                    function->second = translationCount+1;
                    std::pair<int,int> temp = undefinedFunctionMap.find(function->first)->second;
                    bytecode[temp.first] += (std::to_string(function->second) + " " + std::to_string(temp.second));
                    }
                }
            break;
          case TokenType::L_SBACKET:
            bytecode.push_back(OpcodeToString.find(Opcode::NEWSCOPE)->second);
            scopeCounts.push(variableMapByString.size());
            break;
          case TokenType::R_SBACKET:
            {
            bytecode.push_back(OpcodeToString.find(Opcode::ENDSCOPE)->second);
            if (scopeCounts.top() != variableMapByString.size())
            {
                for (int i = scopeCounts.top(); i < variableMapByInt.size(); i++)
                {
                    variableMapByString.erase(variableMapByInt.find(i)->second);
                    variableMapByInt.erase(i);
                } 
            } 
                    
            scopeCounts.pop();
            if (loopPointer.size() > 0 && loopPointer.top().first == scopeCounts.size()) {
                bytecode.push_back(OpcodeToString.find(Opcode::JUMP)->second + " " + std::to_string(loopPointer.top().second));
                loopPointer.pop();
            }

            translationCount=bytecode.size()-1;
            if (tempIfJump.size() > 0 && tempIfJump.top().first == scopeCounts.size())
            {
                int temp = tempIfJump.top().first;
                bytecode[tempIfJump.top().second] += std::to_string(translationCount+1);
                tempIfJump.pop();
                if (tempIfJump.size() > 0 && temp == tempIfJump.top().first)
                {
                    bytecode[tempIfJump.top().second] += std::to_string(translationCount+temp);
                    tempIfJump.pop();
                }
            } 
            
            }
            break;
          case TokenType::RETURN:
                if (tokens.size() == 1) {
                    bytecode.push_back(OpcodeToString.find(Opcode::RETURN)->second + " 0");
                } else {
                    bytecode.push_back(OpcodeToString.find(Opcode::RETURN)->second + " 1");
                }
                
            break;   
          case TokenType::FUNCTION: {
            auto function = functionMap.find(it->code);
            translationCount=bytecode.size()-1;
            if (function == functionMap.end()) {
                functionMap.emplace(it->code, -1);
                bytecode.push_back(OpcodeToString.find(Opcode::CALL)->second + " ");
                undefinedFunctionMap.emplace(it->code, std::make_pair<int,int>(bytecode.size()-1,0));
            } else {
                bytecode.push_back(OpcodeToString.find(Opcode::CALL)->second + " " + std::to_string(function->second) + " " + "0");
            }
          } break; 
          case TokenType::VARIABLIE: {
            auto variable = variableMapByString.find(it->code);
            if (variable == variableMapByString.end()) {
                if (tokens.size() == 1) {
                    variableMapByString.emplace(it->code, variableIndex);
                    variableMapByInt.emplace(variableIndex++, it->code);
                } else throw std::invalid_argument("Unknown variable: " + it->code);
            }
            bytecode.push_back(OpcodeToString.find(Opcode::LOAD)->second + " " +
                               std::to_string(variable->second) + " " + "0");
          } break;
          case TokenType::TRUE:
            bytecode.push_back(
                OpcodeToString.find(Opcode::LOAD_BOOLCONST)->second + " 1");
            break;
          case TokenType::FALSE:
            bytecode.push_back(
                OpcodeToString.find(Opcode::LOAD_BOOLCONST)->second + " 0");
            break;
          case TokenType::NUM_CONST:
            bytecode.push_back(
                OpcodeToString.find(Opcode::LOAD_NUMCONST)->second + " " +
                it->code);
            break;
          case TokenType::WORD_CONST:
            bytecode.push_back(
                OpcodeToString.find(Opcode::LOAD_WORDCONST)->second + " " +
                it->code);
            break;
          case TokenType::EQUAL: {
            it++;
            if (it->tokenType != TokenType::VARIABLIE)
              throw std::invalid_argument("invalid_argument " + it->code);
            auto variable = variableMapByString.find(it->code);
            if (variable == variableMapByString.end()) {
              variableMapByString.emplace(it->code, variableIndex);
              variableMapByInt.emplace(variableIndex, it->code);
              bytecode.push_back(OpcodeToString.find(Opcode::STORE)->second +
                                 " " + std::to_string(variableIndex++) + " " +
                                 "0");
              break;
            }
            bytecode.push_back(OpcodeToString.find(Opcode::STORE)->second +
                               " " + std::to_string(variable->second) + " " +
                               "0");
          } break;
          case TokenType::PLUS:
            bytecode.push_back(OpcodeToString.find(Opcode::ADD)->second);
            break;
          case TokenType::MINUS:
            bytecode.push_back(OpcodeToString.find(Opcode::SUB)->second);
            break;
          case TokenType::TIMES:
            bytecode.push_back(OpcodeToString.find(Opcode::MUL)->second);
            break;
          case TokenType::DIVIDE:
            bytecode.push_back(OpcodeToString.find(Opcode::DIV)->second);
            break;
          case TokenType::MODE:
            bytecode.push_back(OpcodeToString.find(Opcode::MOD)->second);
            break;
          case TokenType::POW:
            bytecode.push_back(OpcodeToString.find(Opcode::POW)->second);
            break;
          case TokenType::AND:
            bytecode.push_back(OpcodeToString.find(Opcode::AND)->second);
            break;
          case TokenType::NOT:
            bytecode.push_back(OpcodeToString.find(Opcode::NOT)->second);
            break;
          case TokenType::XOR:
            bytecode.push_back(OpcodeToString.find(Opcode::XOR)->second);
            break;
          case TokenType::OR:
            bytecode.push_back(OpcodeToString.find(Opcode::OR)->second);
            break;
          case TokenType::GREATER:
            bytecode.push_back(
                OpcodeToString.find(Opcode::GREATER_THAN)->second);
            break;
          case TokenType::SMALLER:
            bytecode.push_back(OpcodeToString.find(Opcode::LESS_THAN)->second);
            break;
          case TokenType::E_GREATER:
            bytecode.push_back(
                OpcodeToString.find(Opcode::GREATER_THAN_EQUAL)->second);
            break;
          case TokenType::E_SMALLER:
            bytecode.push_back(
                OpcodeToString.find(Opcode::LESS_THAN_EQUAL)->second);
            break;
          case TokenType::EQUIVALENT:
            bytecode.push_back(OpcodeToString.find(Opcode::EQUAL)->second);
            break;
          case TokenType::NOT_EQUIVALENT:
            bytecode.push_back(OpcodeToString.find(Opcode::NOT_EQUAL)->second);
            break;
          case TokenType::READ:
            bytecode.push_back(OpcodeToString.find(Opcode::PRINT)->second);
            bytecode.push_back(OpcodeToString.find(Opcode::READ)->second);
            break;
          case TokenType::PRINT:
            bytecode.push_back(OpcodeToString.find(Opcode::PRINT)->second);
            break;
          default:
            break;
        }
        translationCount=bytecode.size()-1;
      }
    } else if (lexingDone) {
      break;
    }
  }
  auto variable = functionMap.find("main");
  if (variable == functionMap.end()) throw std::invalid_argument("Missing main() function!");
}

 void Translator::writeToFile(std::vector<std::string>& bytecode, const std::filesystem::path& outputPath) {
        
        std::ofstream outFile(outputPath);
        for (auto line:bytecode)
        {
            outFile << line << std::endl;
        }
        
        outFile.close();
    }

