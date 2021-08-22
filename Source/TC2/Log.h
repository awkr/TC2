//
// Created by Blue on 2021/6/25.
//

#pragma once

#define NET_MODE                                                               \
  ((!GetWorld())                               ? TEXT("-")                     \
   : GetWorld()->GetNetMode() == NM_Client     ? TEXT("C")                     \
   : GetWorld()->GetNetMode() == NM_Standalone ? TEXT("s")                     \
                                               : TEXT("S"))

#define LOG_WARN(fmt, ...)                                                     \
  UE_LOG(LogTemp, Warning, TEXT("%s [%s:%d] " TEXT(fmt)), NET_MODE,            \
         *FString(__FUNCTION__), __LINE__, ##__VA_ARGS__);
#define LOG_ERROR(fmt, ...)                                                    \
  UE_LOG(LogTemp, Error, TEXT("%s [%s:%d] " TEXT(fmt)), NET_MODE,              \
         *FString(__FUNCTION__), __LINE__, ##__VA_ARGS__);

#define LOG_FUNC_WARN()                                                        \
  UE_LOG(LogTemp, Warning, TEXT("%s [%s:%d] "), NET_MODE,                      \
         *FString(__FUNCTION__), __LINE__);
