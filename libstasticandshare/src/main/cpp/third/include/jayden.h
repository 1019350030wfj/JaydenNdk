//
// Created by jayden on 2020/9/3.
//

#ifndef NATIVEDOING_JAYDEN_H
#define NATIVEDOING_JAYDEN_H

#include <stddef.h>
#include <android/log.h>
#include <jni.h>
#include <string>

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "jayden", __VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    std::string (*getResponse)();
} Jayden_FUN;

class Jayden {
public:
    Jayden(std::string, int);
    std::string getJaydenWord();

private:
    std::string name;
    int age;
};

// attribute GNU C的特色之一
extern __attribute__ ((visibility ("default"))) Jayden_FUN jaydenFun;

#ifdef __cplusplus
}
#endif

#endif //NATIVEDOING_JAYDEN_H
