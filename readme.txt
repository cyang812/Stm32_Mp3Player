This is an old STM32 project.

# Software
Keil
STM32 official BSP/HAL

# Hardware
STM32469I_Discovery_Kits

# AXF compare (for regression debug)
Use this command to compare an old known-good AXF with current build AXF:

powershell -ExecutionPolicy Bypass -File .\scripts\compare_axf.ps1 -OldAxf "D:\github\Stm32_Mp3Player\Old_STM32469I_DISCO.axf" -NewAxf "D:\github\Stm32_Mp3Player\MDK-ARM\STM32469I_DISCO\STM32469I_DISCO.axf" -OutputDir "D:\github\Stm32_Mp3Player\axf_compare"

Or use the cmd wrapper:

.\scripts\compare_axf.cmd "D:\github\Stm32_Mp3Player\Old_STM32469I_DISCO.axf" "D:\github\Stm32_Mp3Player\MDK-ARM\STM32469I_DISCO\STM32469I_DISCO.axf" "D:\github\Stm32_Mp3Player\axf_compare"

