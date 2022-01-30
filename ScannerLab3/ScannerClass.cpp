#include <iostream>
#include <string>
#include "ScannerClass.hpp"


/*
 * Описание функций сканнера
 */

int ScannerClass::get_pointer() const {
    return this->pointer;
}

void ScannerClass::put_pointer(int pointer) {
    this->pointer = pointer;
}


bool ScannerClass::readDataFromFile(const std::string& fileName) {
    std::string textInfo;
    
    FILE* file = fopen(fileName.c_str(), "r");      // Открытие файла в режиме "чтение"

    if (file == nullptr) {
        std::cout << "File is not open" << std::endl;
        std::exit(0);
    }
    int size = 0;
    while (!feof(file)) {
        char c;
        fscanf(file, "%c", &c);
        textInfo.append(std::string(1, c));
        if (++size == Max_Text) //учитываем максимальный размер текста
            break;
    }

    textInfo.append("\0");
    textInfo.erase(textInfo.length() - 1);
    fclose(file);   // Закрываем файл после завершения работы
    this->text_Info.clear();
    this->number_str.clear();
    this->text_Info = textInfo;
    return true;
}

//вывод ошибки
void ScannerClass::show_Error(const std::string& errorMessage) {
    std::cout << errorMessage << std::endl;
}


Lexem ScannerClass::scan() {
    // Значение текущее лексемы
    std::string value;

    //пропускаем пробелы, табуляцию, переходы на другую строку
    while (true) {
        while ((text_Info[pointer] == ' ') || (text_Info[pointer] == '\n') || (text_Info[pointer] == '\t')) {
            if (text_Info[pointer] == '\n') number_Line++;
            pointer++;
        }

        // Пропускаем однострочный коммнентарий
        if ((text_Info[pointer] == '/') && (text_Info[pointer + 1] == '/')) {
            number_Line++;
            pointer += 2;
            while (text_Info[pointer] != '\n'&& text_Info[pointer] != '\0')
                pointer++;
            continue;
        }

        // Пропускаем многострочный комментарий
        if ((text_Info[pointer] == '/') && (text_Info[pointer + 1] == '*')) {
            pointer += 2;
            while ((text_Info[pointer + 1] != '/') || (text_Info[pointer] != '*')) {
                if (text_Info[pointer] == '\n') number_Line++;
                pointer++;
                if (text_Info[pointer + 1] == '\0') return Lexem{ tEnd, value, number_Line};
            }
            pointer += 2;
            continue;
        }

        number_str.push_back(number_Line);
        if (text_Info[pointer] == '\0') {
            text_Info[0] = '\0';
            return Lexem{ tEnd, "END", number_Line };
        }

         /*
          * constInt -> 19
          */

        if ((text_Info[pointer] <= '9') && (text_Info[pointer] >= '1')) {

            value.append(std::string(1, text_Info[pointer++]));
            while ((text_Info[pointer] <= '9') && (text_Info[pointer] >= '0'))
                value.append(std::string(1, text_Info[pointer++]));
            return Lexem{constInt, value, number_Line };

        }
        else         
            /*
          * constHex -> 20
          */
            if ((text_Info[pointer] == '0') && (text_Info[pointer+1] == 'x'|| text_Info[pointer+1] == 'X')) {
            value.append(std::string(1, text_Info[pointer++]));
            value.append(std::string(1, text_Info[pointer++]));
          
                while ((text_Info[pointer] <= '9') && (text_Info[pointer] >= '0') || (text_Info[pointer] >= 'A') && (text_Info[pointer] <= 'F'))
                    value.append(std::string(1, text_Info[pointer++]));
                return Lexem{constHex, value, number_Line };
        }

        else if (text_Info[pointer] == '0' )
        {
                if ((text_Info[pointer + 1] <= '9') && (text_Info[pointer + 1] >= '0'))
                {
                    value.append(std::string(1, text_Info[pointer++]));
                    while ((text_Info[pointer] <= '9') && (text_Info[pointer] >= '0'))
                        value.append(std::string(1, text_Info[pointer++]));
                    std::string message = "Wrong at line: ";
                    message.append(std::to_string(number_Line));
                    message.append("  Error writing a number in 10 ns (extra zero): ");
                    message.append(value);
                    show_Error(message);
                    return Lexem{tError, value, number_Line };
                }
                else
                {//число 0
                    value.append(std::string(1, text_Info[pointer++]));
                    return Lexem{constInt, value, number_Line };
                }
        }
        /*
         * tId -> 1
         */
        else if ((text_Info[pointer] >= 'a') && (text_Info[pointer] <= 'z') || (text_Info[pointer] >= 'A') && (text_Info[pointer] <= 'Z') ||
            (text_Info[pointer] == '_')) {
            value.append(std::string(1, text_Info[pointer++]));
            while (((text_Info[pointer] <= '9') && (text_Info[pointer] >= '0')) || (text_Info[pointer] >= 'a') && (text_Info[pointer] <= 'z') ||
                (text_Info[pointer] >= 'A') && (text_Info[pointer] <= 'Z') || (text_Info[pointer] == '_')) {
                if (value.length() < Max_Lex - 1)
                    value.append(std::string(1, text_Info[pointer++]));
                else
                    pointer++; // Пропускаем длинный идентификатор
            }

            /*
            * Ключевые символы
            */
            for (const auto& keyword : keywords) {
                if (keyword.first == value)
                    return Lexem{ keyword.second, value, number_Line };
            }
            return Lexem{ tId, value, number_Line };
        }


        /*
     * tToch=23
     */
        else if (text_Info[pointer] == '.') {
                value.append(std::string(1, text_Info[pointer++]));
                return Lexem{ tToch, value };
            }
        /*
        * tZpt=24
        */
        else if (text_Info[pointer] == ',') {
            value.append(std::string(1, text_Info[pointer++]));
            return Lexem{ tZpt, value, number_Line };
        }

        /*
    * tDtoch=25
    */
        else if (text_Info[pointer] == ':') {
                value.append(std::string(1, text_Info[pointer++]));
                return Lexem{ tDtoch, value, number_Line };
            }
        /*
        * tTzpt = 26
        */
        else if (text_Info[pointer] == ';') {
            value.append(std::string(1, text_Info[pointer++]));
            return Lexem{ tTzpt, value, number_Line };
        }
        
            
     

        /*
        * tLs = 27
        */
        else if (text_Info[pointer] == '(') {
            value.append(std::string(1, text_Info[pointer++]));
            return Lexem{ tLs, value, number_Line };
        }

        /*
        * tPs = 28
        */
        else if (text_Info[pointer] == ')') {
            value.append(std::string(1, text_Info[pointer++]));
            return Lexem{ tPs, value, number_Line };
        }

        /*
        *  tFls = 29
        */
        else if (text_Info[pointer] == '{') {
            value.append(std::string(1, text_Info[pointer++]));
            return Lexem{ tFls, value, number_Line };
        }

        /*
        *  tFps = 30
        */
        else if (text_Info[pointer] == '}') {
            value.append(std::string(1, text_Info[pointer++]));
            return Lexem{ tFps, value, number_Line };
        }

    
     


        /*
         * tLess = 33
         * tLs = 35
        */
        else if (text_Info[pointer] == '<') {
            value.append(std::string(1, text_Info[pointer++]));

            if (text_Info[pointer] == '=') {
                value.append(std::string(1, text_Info[pointer++]));
                return Lexem{ tLe, value, number_Line };
            }
            return Lexem{ tLess, value, number_Line };
        }

        /*
        * tMore = 34
        * tMe = 36
        */
        else if (text_Info[pointer] == '>') {
            value.append(std::string(1, text_Info[pointer++]));

            if (text_Info[pointer] == '=') {
                value.append(std::string(1, text_Info[pointer++]));
                return Lexem{ tMe, value, number_Line };
            }
            return Lexem{ tMore, value, number_Line };
        }
    
    
        /*
        * tEq = 37
        * tSave = 46
        */
        else if (text_Info[pointer] == '=') {
            value.append(std::string(1, text_Info[pointer++]));

            if (text_Info[pointer] == '=') {
                value.append(std::string(1, text_Info[pointer++]));
                return Lexem{ tEq, value, number_Line };
            }
            return Lexem{ tSave, value, number_Line };
        }


     

        /*

         * tPlus -> 41
         */
        else if (text_Info[pointer] == '+') {
            value.append(std::string(1, text_Info[pointer++]));
            return Lexem{ tPlus, value, number_Line };
        }

        /*
        * tMinus -> 42
        */
        else if (text_Info[pointer] == '-') {
            value.append(std::string(1, text_Info[pointer++]));
            return Lexem{ tMinus, value, number_Line };
        }

        /*
   
         * Div -> 44
        */
        else if (text_Info[pointer] == '/') {
            value.append(std::string(1, text_Info[pointer++]));
            return Lexem{ tDiv, value, number_Line };
        }

        /*
        * tMult = 43
        */
        else if (text_Info[pointer] == '*') {
            value.append(std::string(1, text_Info[pointer++]));


            return Lexem{ tMult, value, number_Line };
        }
   
        /*
        * tNeq = 38
        *  tNot = 45
        */


        else if (text_Info[pointer] == '!') {
            value.append(std::string(1, text_Info[pointer++]));

            if (text_Info[pointer] == '=') {
                value.append(std::string(1, text_Info[pointer++]));
                return Lexem{ tNeq, value, number_Line };
            }
            return Lexem{ tNot, value, number_Line };
        }

        /*
        *  tOr = 39
        */
        else if (text_Info[pointer] == '|'&& text_Info[pointer+1] == '|') {
        value.append(std::string(1, text_Info[pointer++]));
        value.append(std::string(1, text_Info[pointer++]));
        return Lexem{ tOr, value, number_Line };
        }
        /*
     *    tAnd = 40
     */
        else if (text_Info[pointer] == '&' && text_Info[pointer + 1] == '&') {
        value.append(std::string(1, text_Info[pointer++]));
        value.append(std::string(1, text_Info[pointer++]));
        return Lexem{ tAnd, value, number_Line };
        }
       
          

        /*
        * Если не был найден обрабатываемый символ, формируем сообщение об ошибке
        */
        else {
            value.append(std::string(1, text_Info[pointer++]));

            std::string message = "Wrong at line ";
            message.append(std::to_string(number_Line));
            message.append("  Error symbol: ");
            message.append(value);

            show_Error(message);
            return Lexem{tError, value, number_Line };
        }
    }
}




