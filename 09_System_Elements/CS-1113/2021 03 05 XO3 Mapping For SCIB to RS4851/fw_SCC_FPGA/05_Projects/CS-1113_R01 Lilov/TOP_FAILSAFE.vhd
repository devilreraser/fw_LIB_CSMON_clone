library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.std_logic_arith.all;
--library ECP5U;
--use ECP5U.components.all;
--library lattice;
--use lattice.components.all;



entity TOP is


	
	port(
		
		
		--FPGA_GPIO_1V8
		I_O_FPGA_GPIO_1V8			:inout	std_logic_vector(1 to 31);
		
		--RS485_1
		I_FPGA_RS485_1_RX			: in	std_logic;			
		O_FPGA_RS485_1_TX			: out 	std_logic;	
		O_FPGA_RS485_1_DE			: out 	std_logic;
		O_FPGA_RS485_1_RE			: out 	std_logic;
		
		--RS485_2
		I_FPGA_RS485_2_RX			: in	std_logic;			
		O_FPGA_RS485_2_TX			: out 	std_logic;	
		O_FPGA_RS485_2_DE			: out 	std_logic;
		O_FPGA_RS485_2_RE			: out 	std_logic;
		
		--SCIB
		O_DSP_RS485_B_RX			: out 	std_logic;		
		I_DSP_RS485_B_TX			: in 	std_logic;	
		I_DSP_RS485_B_DE			: in 	std_logic;	
		I_DSP_RS485_B_RE			: in 	std_logic;	
		
		--CLOCK
		I_CLOCK_20MHz		  		: in 	std_logic
		
		
		
		
		--I_LVDS					: in std_logic;
		--O_LVDS					: out std_logic;
		--o_DOUT_19				: out std_logic;
		--I_EMIF1_nWE_FP  		: in std_logic;
		--I_EMIF1_nOE_FP  		: in std_logic;
		--I_EMIF1_nCS3_FP  	 	: in std_logic;
		--I_EMIF1_nCS4_FP  	 	: in std_logic;
		--I_EMIF1_ADDRESS_FP	   	: in std_logic_vector(0 to 12);
		--I_EMIF1_EM1BA1_FP		: in std_logic;
		--I_O_EMIF1_DATA_FP	  	: inout std_logic_vector(0 to 15);
		--I_EPWM_1_P				: in std_logic;
		--I_EPWM_1_N				: in std_logic;
		--I_EPWM_2_P				: in std_logic;
		--I_EPWM_2_N				: in std_logic;
		--I_EPWM_3_P				: in std_logic;
		--I_EPWM_3_N				: in std_logic;
		--I_EPWM_4_P				: in std_logic;
		--I_EPWM_4_N				: in std_logic;
		--I_EPWM_5_P				: in std_logic;
		--I_EPWM_5_N				: in std_logic;
		--I_EPWM_6_P				: in std_logic;
		--I_EPWM_6_N				: in std_logic;
		--I_EPWM_7_P				: in std_logic;
		--I_EPWM_7_N				: in std_logic;
		--I_EPWM_8_P				: in std_logic;
		--I_EPWM_8_N				: in std_logic;
		--I_EPWM_9_P				: in std_logic;
		--I_EPWM_9_N				: in std_logic;
		--I_EPWM_10_P				: in std_logic;
		--I_EPWM_10_N				: in std_logic;
		--I_EPWM_11_P				: in std_logic;
		--I_EPWM_11_N				: in std_logic;
		--I_EPWM_12_P				: in std_logic;
		--I_EPWM_12_N				: in std_logic;
		--O_PWM_1_U_TOP			: out std_logic;
		--O_PWM_1_U_BOT			: out std_logic;
		--O_PWM_1_V_TOP			: out std_logic;
		--O_PWM_1_V_BOT			: out std_logic;
		--O_PWM_1_W_TOP			: out std_logic;
		--O_PWM_1_W_BOT			: out std_logic;
		--O_PWM_2_U_TOP			: out std_logic;
		--O_PWM_2_U_BOT			: out std_logic;
		--O_PWM_2_V_TOP			: out std_logic;
		--O_PWM_2_V_BOT			: out std_logic;
		--O_PWM_2_W_TOP			: out std_logic;
		--O_PWM_2_W_BOT			: out std_logic;
		--O_PWM_3_U_TOP			: out std_logic;
		--O_PWM_3_U_BOT			: out std_logic;
		--O_PWM_3_V_TOP			: out std_logic;
		--O_PWM_3_V_BOT			: out std_logic;
		--O_PWM_3_W_TOP			: out std_logic;
		--O_PWM_3_W_BOT			: out std_logic;
		--O_PWM_4_U_TOP			: out std_logic;
		--O_PWM_4_U_BOT			: out std_logic;
		--O_PWM_4_V_TOP			: out std_logic;
		--O_PWM_4_V_BOT			: out std_logic;
		--O_PWM_4_W_TOP			: out std_logic;
		--O_PWM_4_W_BOT			: out std_logic;
		--I_O_GPIO_FPGA_1V8		: inout std_logic_vector(0 to 30);
		--I_O_GPIO_FPGA_2V5		: inout std_logic_vector(0 to 11);
		--I_O_LVDS				: inout std_logic_vector(0 to 2)
		--O_DSP_RESET				: out std_logic;
		--I_O_BM_SEL0_SCITXDA			: inout std_logic;
		--O_PROGRAMN					: out std_logic;
		--O_MCLK						: out std_logic;
		--I_O_PWM_BUF					: inout std_logic_vector(0 to 5);
		--I_O_FLT						: inout std_logic_vector(0 to 2);
		--O_RTC_nRESET				: out std_logic;
		--O_RTC_SCL					: out std_logic;
		--I_O_RTC_SDA					: inout std_logic;
		--O_FPGA_EEPROM_CLK			: out std_logic;
		--O_FPGA_EEPROM_SIMO			: out std_logic;
		--I_FPGA_EEPROM_SOMI			: in std_logic;
		--O_FPGA_EEPROM_nCS			: out std_logic;
		--I_O_DSP_EQEP1A_FPGA			: inout std_logic;
		--I_O_DSP_EQEP1B_FPGA			: inout std_logic;
		--I_O_DSP_EQEP1S_FPGA			: inout std_logic;
		--I_O_DSP_EQEP1I_FPGA			: inout std_logic;
		--I_O_DSP_SPI_C_SIMO_FPGA		: inout std_logic;
		--I_O_DSP_SPI_C_SOMI_FPGA		: inout std_logic;
		--I_O_DSP_SPI_C_CLK_FPGA		: inout std_logic;
		--I_O_DSP_SPI_C_TE0_FPGA		: inout std_logic;
		--O_DSP_SCI_D_RX_FPGA			: out std_logic;
		--I_DSP_SCI_D_TX_FPGA			: in std_logic;
		--I_O_DSP_EQEP2A_FPGA			: inout std_logic;
		--I_O_DSP_EQEP2B_FPGA			: inout std_logic;
		--I_O_DSP_EQEP2S_FPGA			: inout std_logic;
		--I_O_DSP_EQEP2I_FPGA			: inout std_logic;		
		--I_O_DSP_IO_1_FPGA			: inout std_logic;
		--I_O_DSP_IO_2_FPGA			: inout std_logic;
		--I_O_DSP_IO_3_FPGA			: inout std_logic;	
		--I_O_WROOM_SPI_SDI			: inout std_logic;
		--I_O_WROOM_SPI_CLK			: inout std_logic;
		--I_O_WROOM_SPI_CS			: inout std_logic;
		--I_O_WROOM_SPI_SDO			: inout std_logic;	
		--I_O_WROOM_U0RXD				: inout std_logic;
		--I_O_WROOM_U0TXD				: inout std_logic;
		--I_O_WROOM_GPIO0				: inout std_logic;
		--I_ANYBUS_nIRQ				: in std_logic;
		--O_ANYBUS_nRESET				: out std_logic;
		--I_PIC16_UART_TX				: in std_logic;
		--O_PIC16_UART_RX				: out std_logic;
		--I_DSP_ERROR_RESET_FPGA		: in std_logic
		);
	end TOP;
 

architecture rtl of TOP is





--signal definitions

signal sUsedClock : std_logic;
signal s_ts : std_logic;
signal s_test : std_logic;
signal s_counter : integer := 0;
--signal s_Buffer_GPIO : std_logic_vector(1 to 31);
signal s_Buffer_LVDS : std_logic_vector(0 to 2);
--signal s_Buffer_LVCMOS25 : std_logic_vector(0 to 11);
--signal s_general_buffer : std_logic_vector(0 to 15);


begin

		sUsedClock <= I_CLOCK_20MHz;

-- PWM signals passing-through

		-- SCIB	<-> RS485_1
		--O_DSP_RS485_B_RX <= I_FPGA_RS485_1_RX or I_DSP_RS485_B_RE;		-- I_DSP_RS485_B_RE always set to zero from DSP
		--O_FPGA_RS485_1_TX <= I_DSP_RS485_B_TX;
		--O_FPGA_RS485_1_DE <= I_DSP_RS485_B_DE;
		--O_FPGA_RS485_1_RE <= 'Z';										-- O_FPGA_RS485_1_RE not used (zero Ohm rezistor between O_FPGA_RS485_1_DE and O_FPGA_RS485_1_DE)
		
		-- SCIB	<-> RS485_2
		O_DSP_RS485_B_RX <=	I_FPGA_RS485_2_RX or I_DSP_RS485_B_RE;		-- I_DSP_RS485_B_RE always set to zero from DSP
		O_FPGA_RS485_2_TX <= I_DSP_RS485_B_TX;
		O_FPGA_RS485_2_DE <= I_DSP_RS485_B_DE;
		O_FPGA_RS485_2_RE <= 'Z';										-- O_FPGA_RS485_2_RE not used (zero Ohm rezistor between O_FPGA_RS485_2_DE and O_FPGA_RS485_2_DE)
		




		--O_PWM_1_U_TOP <= I_EPWM_1_P or I_EMIF1_nWE_FP or I_EMIF1_nOE_FP or I_EMIF1_nCS3_FP or I_EMIF1_nCS4_FP or I_EMIF1_EM1BA1_FP;
		--O_PWM_1_U_BOT <= I_EPWM_1_N or I_EMIF1_ADDRESS_FP(0) or I_EMIF1_ADDRESS_FP(1) or I_EMIF1_ADDRESS_FP(2) or I_EMIF1_ADDRESS_FP(3);
		--O_PWM_1_V_TOP <= I_EPWM_2_P or I_EMIF1_ADDRESS_FP(4) or I_EMIF1_ADDRESS_FP(5) or I_EMIF1_ADDRESS_FP(6) or I_EMIF1_ADDRESS_FP(7) or I_EMIF1_ADDRESS_FP(8);
		--O_PWM_1_V_BOT <= I_EPWM_2_N or I_EMIF1_ADDRESS_FP(9) or I_EMIF1_ADDRESS_FP(10) or I_EMIF1_ADDRESS_FP(11) or I_EMIF1_ADDRESS_FP(12);
		--O_PWM_1_W_TOP <= I_EPWM_3_P;
		--O_PWM_1_W_BOT <= I_EPWM_3_N or I_O_BM_SEL0_SCITXDA or I_O_EMIF1_DATA_FP(0) or I_O_EMIF1_DATA_FP(1) or I_O_EMIF1_DATA_FP(2) or I_O_EMIF1_DATA_FP(3) or I_O_EMIF1_DATA_FP(4);
		--O_PWM_2_U_TOP <= I_EPWM_4_P or I_O_EMIF1_DATA_FP(5) or I_O_EMIF1_DATA_FP(6) or I_O_EMIF1_DATA_FP(7) or I_O_EMIF1_DATA_FP(8) or I_O_EMIF1_DATA_FP(9) or I_O_EMIF1_DATA_FP(10);
		--O_PWM_2_U_BOT <= I_EPWM_4_N or I_O_EMIF1_DATA_FP(11) or I_O_EMIF1_DATA_FP(12) or I_O_EMIF1_DATA_FP(13) or I_O_EMIF1_DATA_FP(13) or I_O_EMIF1_DATA_FP(14) or I_O_EMIF1_DATA_FP(15);
		--O_PWM_2_V_TOP <= I_EPWM_5_P or I_O_PWM_BUF(0) or I_O_PWM_BUF(1) or I_O_PWM_BUF(2) or I_O_PWM_BUF(3) or I_O_PWM_BUF(4) or I_O_PWM_BUF(5);
		--O_PWM_2_V_BOT <= I_EPWM_5_N or I_O_FLT(0) or I_O_FLT(1) or I_O_FLT(2);
		--O_PWM_2_W_TOP <= I_EPWM_6_P or I_O_DSP_IO_1_FPGA or I_O_DSP_IO_2_FPGA or I_O_DSP_IO_3_FPGA;
		--O_PWM_2_W_BOT <= I_EPWM_6_N or I_O_PWM_BUF(0) or I_O_PWM_BUF(1) or I_O_PWM_BUF(2);
		--O_PWM_3_U_TOP <= I_EPWM_7_P or I_O_WROOM_GPIO0 or I_ANYBUS_nIRQ;
		--O_PWM_3_U_BOT <= I_EPWM_7_N or I_O_PWM_BUF(3) or I_O_PWM_BUF(4) or I_O_PWM_BUF(5);
		--O_PWM_3_V_TOP <= I_EPWM_8_P or I_DSP_ERROR_RESET_FPGA;
		--O_PWM_3_V_BOT <= I_EPWM_8_N;
		--O_PWM_3_W_TOP <= I_EPWM_9_P;
		--O_PWM_3_W_BOT <= I_EPWM_9_N;
		--O_PWM_4_U_TOP <= I_EPWM_10_P;
		--O_PWM_4_U_BOT <= I_EPWM_10_N;
		--O_PWM_4_V_TOP <= I_EPWM_11_P;
		--O_PWM_4_V_BOT <= I_EPWM_11_N;
		--O_PWM_4_W_TOP <= I_EPWM_12_P;
		--O_PWM_4_W_BOT <= I_EPWM_12_N;	
		--I_O_DSP_EQEP1A_FPGA <= I_O_DSP_EQEP2A_FPGA;
		--I_O_DSP_EQEP1B_FPGA <= I_O_DSP_EQEP2B_FPGA;
		--I_O_DSP_EQEP1S_FPGA <= I_O_DSP_EQEP2S_FPGA;
		--I_O_DSP_EQEP1I_FPGA <= I_O_DSP_EQEP2I_FPGA;
		--I_O_DSP_SPI_C_SIMO_FPGA <= I_O_DSP_SPI_C_SOMI_FPGA;
		--O_DSP_SCI_D_RX_FPGA <= I_DSP_SCI_D_TX_FPGA;
		--I_O_WROOM_SPI_SDI <= I_O_WROOM_SPI_SDO or I_O_WROOM_SPI_CS or I_O_WROOM_SPI_CLK;
		--I_O_WROOM_U0RXD <= I_O_WROOM_U0TXD;
		--O_PIC16_UART_RX <= I_PIC16_UART_TX;
		
		--O_LVDS <= I_LVDS;
		--s_ts <= I_LVDS;
		
		
--O_DSP_RESET <= 'Z';
--O_PROGRAMN <= 'Z';
--O_RTC_nRESET <= 'Z';
--O_RTC_SCL <= '1';
--O_FPGA_EEPROM_CLK <= s_test;
--O_FPGA_EEPROM_SIMO <= I_FPGA_EEPROM_SOMI;
--O_FPGA_EEPROM_nCS <= '0';
--O_MCLK <= 'Z';
--I_O_DSP_SPI_C_TE0_FPGA <= '0';
--I_O_DSP_SPI_C_CLK_FPGA <= s_test;
--O_ANYBUS_nRESET <= 'Z';


						
--I_O_GPIO_FPGA_2V5 <= s_Buffer_LVCMOS25 when s_test = '0' else -- writing data to BLVDS BUS 1 
				 --(OTHERS => 'Z');
--s_Buffer_LVCMOS25 <= I_O_GPIO_FPGA_2V5 when s_test = '1' else -- writing data to BLVDS BUS 2
						--(OTHERS => 'Z');

--I_O_LVDS <= s_Buffer_LVDS when s_test = '0' else -- writing data to BLVDS BUS 1 
--				 (OTHERS => 'Z');
--s_Buffer_LVDS <= I_O_LVDS when s_test = '1' else -- writing data to BLVDS BUS 2
--						(OTHERS => 'Z');

--I_O_RTC_SDA <= s_general_buffer(1) when s_test = '0' else -- writing data to BLVDS BUS 1 
				 --'1';
--s_general_buffer(1) <= I_O_RTC_SDA when s_test = '1' else -- writing data to BLVDS BUS 2
						--'1';


P_Clock_DIV_1 : PROCESS(sUsedClock)
BEGIN
	IF rising_edge(sUsedClock) THEN
		IF s_counter = 600 THEN
			s_test <= not s_test;
			s_counter <= 0;
			--O_LVDS <= '0';
			I_O_FPGA_GPIO_1V8 <= (OTHERS => s_test);
		ELSE
			s_counter <= s_counter + 1;
			--O_LVDS <= '1';
			I_O_FPGA_GPIO_1V8 <= (OTHERS => 'Z');
		END IF;
		
	END IF;
END PROCESS;



	


	
end rtl;