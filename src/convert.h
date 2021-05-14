#pragma once

enum class EFileType
{
    KCL,
    PMP
};

void convertFile(char*, EFileType);
