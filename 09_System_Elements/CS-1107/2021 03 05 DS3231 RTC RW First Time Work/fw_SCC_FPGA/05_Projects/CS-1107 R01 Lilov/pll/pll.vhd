-- VHDL netlist generated by SCUBA Diamond (64-bit) 3.11.3.469
-- Module  Version: 5.7
--C:\lscc\diamond\3.11_x64\ispfpga\bin\nt64\scuba.exe -w -n pll -lang vhdl -synth lse -bus_exp 7 -bb -arch sa5p00 -type pll -fin 20.00 -clkibuf LVCMOS33 -fclkop 100.00 -fclkop_tol 0.0 -fclkos 20.00 -fclkos_tol 0.0 -phases 0 -fclkos2 100.00 -fclkos2_tol 0.0 -phases2 0 -fclkos3 100.00 -fclkos3_tol 0.0 -phases3 0 -phase_cntl STATIC -fb_mode 5 -fdc D:/repo_git/fw_SCC_FPGA/05_Projects/CS-1107 R01 Lilov/pll/pll.fdc 

-- Thu Mar 04 15:28:17 2021

library IEEE;
use IEEE.std_logic_1164.all;
library ECP5U;
use ECP5U.components.all;

entity pll is
    port (
        CLKI: in  std_logic; 
        CLKOP: out  std_logic; 
        CLKOS: out  std_logic; 
        CLKOS2: out  std_logic; 
        CLKOS3: out  std_logic);
end pll;

architecture Structure of pll is

    -- internal signal declarations
    signal REFCLK: std_logic;
    signal LOCK: std_logic;
    signal CLKOS3_t: std_logic;
    signal CLKOS2_t: std_logic;
    signal CLKOS_t: std_logic;
    signal CLKOP_t: std_logic;
    signal CLKFB_t: std_logic;
    signal buf_CLKI: std_logic;
    signal scuba_vhi: std_logic;
    signal scuba_vlo: std_logic;

    attribute IO_TYPE : string; 
    attribute FREQUENCY_PIN_CLKOS3 : string; 
    attribute FREQUENCY_PIN_CLKOS2 : string; 
    attribute FREQUENCY_PIN_CLKOS : string; 
    attribute FREQUENCY_PIN_CLKOP : string; 
    attribute FREQUENCY_PIN_CLKI : string; 
    attribute ICP_CURRENT : string; 
    attribute LPF_RESISTOR : string; 
    attribute IO_TYPE of Inst1_IB : label is "LVCMOS33";
    attribute FREQUENCY_PIN_CLKOS3 of PLLInst_0 : label is "100.000000";
    attribute FREQUENCY_PIN_CLKOS2 of PLLInst_0 : label is "100.000000";
    attribute FREQUENCY_PIN_CLKOS of PLLInst_0 : label is "20.000000";
    attribute FREQUENCY_PIN_CLKOP of PLLInst_0 : label is "100.000000";
    attribute FREQUENCY_PIN_CLKI of PLLInst_0 : label is "20.000000";
    attribute ICP_CURRENT of PLLInst_0 : label is "5";
    attribute LPF_RESISTOR of PLLInst_0 : label is "16";
    attribute syn_keep : boolean;
    attribute NGD_DRC_MASK : integer;
    attribute NGD_DRC_MASK of Structure : architecture is 1;

begin
    -- component instantiation statements
    Inst1_IB: IB
        port map (I=>CLKI, O=>buf_CLKI);

    scuba_vhi_inst: VHI
        port map (Z=>scuba_vhi);

    scuba_vlo_inst: VLO
        port map (Z=>scuba_vlo);

    PLLInst_0: EHXPLLL
        generic map (PLLRST_ENA=> "DISABLED", INTFB_WAKE=> "DISABLED", 
        STDBY_ENABLE=> "DISABLED", DPHASE_SOURCE=> "DISABLED", 
        CLKOS3_FPHASE=>  0, CLKOS3_CPHASE=>  5, CLKOS2_FPHASE=>  0, 
        CLKOS2_CPHASE=>  5, CLKOS_FPHASE=>  0, CLKOS_CPHASE=>  29, 
        CLKOP_FPHASE=>  0, CLKOP_CPHASE=>  5, PLL_LOCK_MODE=>  0, 
        CLKOS_TRIM_DELAY=>  0, CLKOS_TRIM_POL=> "FALLING", 
        CLKOP_TRIM_DELAY=>  0, CLKOP_TRIM_POL=> "FALLING", 
        OUTDIVIDER_MUXD=> "DIVD", CLKOS3_ENABLE=> "ENABLED", 
        OUTDIVIDER_MUXC=> "DIVC", CLKOS2_ENABLE=> "ENABLED", 
        OUTDIVIDER_MUXB=> "DIVB", CLKOS_ENABLE=> "ENABLED", 
        OUTDIVIDER_MUXA=> "DIVA", CLKOP_ENABLE=> "ENABLED", CLKOS3_DIV=>  6, 
        CLKOS2_DIV=>  6, CLKOS_DIV=>  30, CLKOP_DIV=>  6, CLKFB_DIV=>  5, 
        CLKI_DIV=>  1, FEEDBK_PATH=> "INT_OP")
        port map (CLKI=>buf_CLKI, CLKFB=>CLKFB_t, PHASESEL1=>scuba_vlo, 
            PHASESEL0=>scuba_vlo, PHASEDIR=>scuba_vlo, 
            PHASESTEP=>scuba_vlo, PHASELOADREG=>scuba_vlo, 
            STDBY=>scuba_vlo, PLLWAKESYNC=>scuba_vlo, RST=>scuba_vlo, 
            ENCLKOP=>scuba_vlo, ENCLKOS=>scuba_vlo, ENCLKOS2=>scuba_vlo, 
            ENCLKOS3=>scuba_vlo, CLKOP=>CLKOP_t, CLKOS=>CLKOS_t, 
            CLKOS2=>CLKOS2_t, CLKOS3=>CLKOS3_t, LOCK=>LOCK, 
            INTLOCK=>open, REFCLK=>REFCLK, CLKINTFB=>CLKFB_t);

    CLKOS3 <= CLKOS3_t;
    CLKOS2 <= CLKOS2_t;
    CLKOS <= CLKOS_t;
    CLKOP <= CLKOP_t;
end Structure;
