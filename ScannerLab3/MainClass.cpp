#include <iostream>
#include "ScannerClass.hpp"
#include "Analisys.hpp"

int main(int argc, char* argv[]) {
    setlocale(0, "");
    std::string fileName;

    if (argc > 1) {
        fileName = argv[1];
    }
    else {
        fileName = "input.txt";
    }

    /*
     * Создаём объект анализатора и передаём ему имя файла
     */
    Analisys analyser(fileName);

    SemTree* st = new SemTree();
    st->SemFinishFunc(st);
    analyser.setSemTree(st);
    /*
     * Запускаем анализатор и выводим соотвествующие результат сообщения
     */
    try {
        std::cout << "START" << std::endl;
        analyser.analysis();
        std::cout << "Successfully!" << std::endl;

    }
    catch (std::runtime_error& error) {
        std::cout << error.what();
        std::cout << "*Errors*" << std::endl;
    }
    std::cout << "FINISH" << std::endl;
  
    st->Print();
    return 0;
}