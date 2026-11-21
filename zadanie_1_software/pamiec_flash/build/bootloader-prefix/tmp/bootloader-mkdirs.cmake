# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/maciej-grylewicz/esp/esp-idf/components/bootloader/subproject"
  "/home/maciej-grylewicz/Dokumenty/projekty_esp32/rekrutacja_inspace/zadanie_1_software/pamiec_flash/build/bootloader"
  "/home/maciej-grylewicz/Dokumenty/projekty_esp32/rekrutacja_inspace/zadanie_1_software/pamiec_flash/build/bootloader-prefix"
  "/home/maciej-grylewicz/Dokumenty/projekty_esp32/rekrutacja_inspace/zadanie_1_software/pamiec_flash/build/bootloader-prefix/tmp"
  "/home/maciej-grylewicz/Dokumenty/projekty_esp32/rekrutacja_inspace/zadanie_1_software/pamiec_flash/build/bootloader-prefix/src/bootloader-stamp"
  "/home/maciej-grylewicz/Dokumenty/projekty_esp32/rekrutacja_inspace/zadanie_1_software/pamiec_flash/build/bootloader-prefix/src"
  "/home/maciej-grylewicz/Dokumenty/projekty_esp32/rekrutacja_inspace/zadanie_1_software/pamiec_flash/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/maciej-grylewicz/Dokumenty/projekty_esp32/rekrutacja_inspace/zadanie_1_software/pamiec_flash/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/maciej-grylewicz/Dokumenty/projekty_esp32/rekrutacja_inspace/zadanie_1_software/pamiec_flash/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
