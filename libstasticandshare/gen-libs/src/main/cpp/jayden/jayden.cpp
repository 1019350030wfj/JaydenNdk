//
// Created by jayden on 2020/9/3.
//

#include "jayden.h"
#include <stdio.h>

Jayden::Jayden(std::string name, int age) {
    this->age = age;
    this->name = name;
}

std::string Jayden::getJaydenWord() {
    char tem[3];
    sprintf(tem, "%d", age);
    return ("Hello my name is " + name + " I am " + tem + " years old");
}

std::string get_response() {
    Jayden jayden = Jayden("Jayden", 29);
    return jayden.getJaydenWord();
}

__attribute__ ((visibility ("default"))) Jayden_FUN jaydenFun = {
        get_response
};
