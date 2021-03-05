library ieee;
use ieee.std_logic_1164.all;
--use ieee.std_logic_unsigned.all;
--use ieee.std_logic_arith.all;
use ieee.numeric_std.all;
library ECP5U;
use ECP5U.components.all;
--library lattice;
--use lattice.components.all;


entity TOP is

	generic (
		I2C_BUFFER_SIZE			: integer := 32
		);
		
	port(
		I_OSC_20MHZ		  		: in std_logic;
		I_EMIF1_nWE_FP  		: in std_logic;
		I_EMIF1_nOE_FP  		: in std_logic;
		I_EMIF1_nCS3_FP  	 	: in std_logic;
		I_EMIF1_nCS4_FP  	 	: in std_logic;
		I_EMIF1_ADDRESS_FP	   	: in std_logic_vector(12 downto 0);
		I_EMIF1_EM1BA1_FP		: in std_logic;
		I_O_EMIF1_DATA_FP	  	: inout std_logic_vector(15 downto 0);
		I_EPWM_1_P				: in std_logic;
		I_EPWM_1_N				: in std_logic;
		I_EPWM_2_P				: in std_logic;
		I_EPWM_2_N				: in std_logic;
		I_EPWM_3_P				: in std_logic;
		I_EPWM_3_N				: in std_logic;
		I_EPWM_4_P				: in std_logic;
		I_EPWM_4_N				: in std_logic;
		I_EPWM_5_P				: in std_logic;
		I_EPWM_5_N				: in std_logic;
		I_EPWM_6_P				: in std_logic;
		I_EPWM_6_N				: in std_logic;
		I_EPWM_7_P				: in std_logic;
		I_EPWM_7_N				: in std_logic;
		I_EPWM_8_P				: in std_logic;
		I_EPWM_8_N				: in std_logic;
		I_EPWM_9_P				: in std_logic;
		I_EPWM_9_N				: in std_logic;
		I_EPWM_10_P				: in std_logic;
		I_EPWM_10_N				: in std_logic;
		I_EPWM_11_P				: in std_logic;
		I_EPWM_11_N				: in std_logic;
		I_EPWM_12_P				: in std_logic;
		I_EPWM_12_N				: in std_logic;
		O_PWM_1_U_TOP			: out std_logic;
		O_PWM_1_U_BOT			: out std_logic;
		O_PWM_1_V_TOP			: out std_logic;
		O_PWM_1_V_BOT			: out std_logic;
		O_PWM_1_W_TOP			: out std_logic;
		O_PWM_1_W_BOT			: out std_logic;
		O_PWM_2_U_TOP			: out std_logic;
		O_PWM_2_U_BOT			: out std_logic;
		O_PWM_2_V_TOP			: out std_logic;
		O_PWM_2_V_BOT			: out std_logic;
		O_PWM_2_W_TOP			: out std_logic;
		O_PWM_2_W_BOT			: out std_logic;
		O_PWM_3_U_TOP			: out std_logic;
		O_PWM_3_U_BOT			: out std_logic;
		O_PWM_3_V_TOP			: out std_logic;
		O_PWM_3_V_BOT			: out std_logic;
		O_PWM_3_W_TOP			: out std_logic;
		O_PWM_3_W_BOT			: out std_logic;
		O_PWM_4_U_TOP			: out std_logic;
		O_PWM_4_U_BOT			: out std_logic;
		O_PWM_4_V_TOP			: out std_logic;
		O_PWM_4_V_BOT			: out std_logic;
		O_PWM_4_W_TOP			: out std_logic;
		O_PWM_4_W_BOT			: out std_logic;
		O_GPIO_FPGA_1V8		: out std_logic_vector(0 to 30);
		I_O_GPIO_FPGA_2V5		: inout std_logic_vector(0 to 11);
		I_O_LVDS				: inout std_logic_vector(0 to 7);
		O_DSP_RESET				: out std_logic;
		I_O_BM_SEL0_SCITXDA			: inout std_logic;
		O_PROGRAMN					: out std_logic;
		O_MCLK						: out std_logic;
		I_O_PWM_BUF					: inout std_logic_vector(0 to 5);
		I_O_FLT						: inout std_logic_vector(0 to 2);
		
		I_O_RTC_nRESET				: inout std_logic;
		I_RTC_SCL					: in std_logic;
		I_O_RTC_SDA					: inout std_logic;
		
		O_FPGA_EEPROM_CLK			: out std_logic;
		O_FPGA_EEPROM_SIMO			: out std_logic;
		I_FPGA_EEPROM_SOMI			: in std_logic;
		O_FPGA_EEPROM_nCS			: out std_logic;
		I_O_DSP_EQEP1A_FPGA			: inout std_logic;
		I_O_DSP_EQEP1B_FPGA			: inout std_logic;
		I_O_DSP_EQEP1S_FPGA			: inout std_logic;
		I_O_DSP_EQEP1I_FPGA			: inout std_logic;
		I_O_DSP_SPI_C_SIMO_FPGA		: inout std_logic;
		I_O_DSP_SPI_C_SOMI_FPGA		: inout std_logic;
		I_O_DSP_SPI_C_CLK_FPGA		: inout std_logic;
		I_O_DSP_SPI_C_TE0_FPGA		: inout std_logic;
		O_DSP_SCI_D_RX_FPGA			: out std_logic;
		I_DSP_SCI_D_TX_FPGA			: in std_logic;
		I_O_DSP_EQEP2A_FPGA			: inout std_logic;
		I_O_DSP_EQEP2B_FPGA			: inout std_logic;
		I_O_DSP_EQEP2S_FPGA			: inout std_logic;
		I_O_DSP_EQEP2I_FPGA			: inout std_logic;		
		I_O_WROOM_SPI_SDI			: inout std_logic;
		I_O_WROOM_SPI_CLK			: inout std_logic;
		I_O_WROOM_SPI_CS			: inout std_logic;
		I_O_WROOM_SPI_SDO			: inout std_logic;	
		I_ANYBUS_nIRQ				: in std_logic;
		O_ANYBUS_nRESET				: out std_logic;
		I_PIC16_UART_TX				: in std_logic;
		O_PIC16_UART_RX				: out std_logic;
		I_DSP_ERROR_RESET_FPGA		: in std_logic;
		O_WROOM_EN					: out std_logic;
		O_WROOM_GPIO0				: out std_logic;
		I_WROOM_U0RXD_FPGA			: in std_logic;
		O_WROOM_U0TXD_FPGA			: out std_logic;
		O_WROOM_U0RXD				: out std_logic;
		I_WROOM_U0TXD				: in std_logic
	
		);
	end TOP;
 

architecture rtl of TOP is


--OSC Signals
signal s_OSC_internal : std_logic := '0';
signal s_OSC_20MHz : std_logic := '0';
signal s_OSC_100MHz1 : std_logic := '0';
signal s_OSC_100MHz2 : std_logic := '0';
signal s_OSC_100MHz3 : std_logic := '0';

-- I2C Address
signal s_I2C_wr_addr : natural range 0 to (I2C_BUFFER_SIZE - 1) := 0;
signal s_I2C_wr_addr_memory : natural range 0 to (I2C_BUFFER_SIZE - 1) := 0;
signal s_I2C_rd_addr_memory : natural range 0 to (I2C_BUFFER_SIZE - 1) := 0;
signal s_I2C_wr_addr_vector : std_logic_vector(4 downto 0) := (OTHERS => '0');
signal s_I2C_rd_addr_vector : std_logic_vector(4 downto 0)	:= (OTHERS => '0');

--I2C Slave Signals
signal s_I2C_XRESET_VECTOR : std_logic_vector(0 to 3) := "1111";
signal s_I2C_XRESET : std_logic := '1';
signal s_I2C_ready : std_logic := '0';
signal s_I2C_start : std_logic;
signal s_I2C_stop : std_logic;
signal s_I2C_data_in : std_logic_vector(7 DOWNTO 0) := "11111111";  -- parallel data in
signal s_I2C_data_out: std_logic_vector(7 DOWNTO 0);  -- parallel data out
signal s_I2C_r_w : std_logic;
signal s_I2C_data_vld : std_logic;
signal s_I2C_scl_oe : std_logic := '0';
signal s_I2C_sda_read : std_logic;

signal s_I2C_sda_oe : std_logic := '0';
signal s_I2C_sda_oe_en:std_logic := '0';

--I2C Signals Edge Detect
signal s_I2C_data_vld_detect : std_logic;
signal s_I2C_start_detect : std_logic;
signal s_I2C_sda_oe_detect : std_logic;
signal s_I2C_stop_detect : std_logic;

--I2C State Machine
constant idle   	: std_logic_vector(1 DOWNTO 0) := "00";
constant start  	: std_logic_vector(1 DOWNTO 0) := "01";
constant completed	: std_logic_vector(1 DOWNTO 0) := "10";
constant reserved  	: std_logic_vector(1 DOWNTO 0) := "11";
signal s_I2C_state : std_logic_vector(1 DOWNTO 0) := completed;     -- state machine

--EBR (I2C -> EMIF)
signal s_RAM_DataInA : std_logic_vector(15 downto 0) := (OTHERS => '0');
signal s_RAM_DataInB : std_logic_vector(7 downto 0) := (OTHERS => '0');
signal s_RAM_AddressA : std_logic_vector(3 downto 0) := (OTHERS => '0');
signal s_RAM_AddressB : std_logic_vector(4 downto 0) := (OTHERS => '0');
signal s_RAM1_QA : std_logic_vector(15 downto 0) := (OTHERS => '0');
signal s_RAM1_QB : std_logic_vector(7 downto 0) := (OTHERS => '0');
signal s_RAM2_QA : std_logic_vector(15 downto 0) := (OTHERS => '0');
signal s_RAM2_QB : std_logic_vector(7 downto 0) := (OTHERS => '0');
signal s_RAM3_QA : std_logic_vector(15 downto 0) := (OTHERS => '0');
signal s_RAM3_QB : std_logic_vector(7 downto 0) := (OTHERS => '0');
signal s_RAM4_QA : std_logic_vector(15 downto 0) := (OTHERS => '0');
signal s_RAM4_QB : std_logic_vector(7 downto 0) := (OTHERS => '0');

signal s_EMIF1_WE_FP : std_logic;
signal s_I2C_RAM_WE : std_logic;

signal s_RAM2_EmifRead : std_logic := '0';	-- 0 - RAM1 Emif Read  | 1 - RAM2 Emif Read
signal s_RAM4_EmifWrite: std_logic := '0';	-- 0 - RAM3 Emif Write | 1 - RAM4 Emif Write
signal s_RAM1_ResetA:std_logic := '0';
signal s_RAM2_ResetA:std_logic := '1';
signal s_RAM1_ResetB:std_logic := '1';
signal s_RAM2_ResetB:std_logic := '0';

signal s_RAM3_ResetA:std_logic := '0';
signal s_RAM4_ResetA:std_logic := '1';
signal s_RAM3_ResetB:std_logic := '1';
signal s_RAM4_ResetB:std_logic := '0';

signal s_I2C_RAM3_Busy  : std_logic := '0';
signal s_I2C_RAM4_Busy  : std_logic := '0';

signal s_I2C_RAM1_Valid : std_logic := '1';
signal s_I2C_RAM2_Valid : std_logic := '0';
signal s_I2C_RAM3_Valid : std_logic := '0';
signal s_I2C_RAM4_Valid : std_logic := '0';

signal s_EMIF_Read:std_logic := '0';

signal s_RAM2_EmifRead_Last:std_logic := '0';
signal s_RAM2_EmifRead_Previous:std_logic := '0';

signal s_EmifWriteValidFlag:std_logic := '0';

--COMPONENTS Start

--component OSCG
--Port (OSC : out STD_LOGIC);
--end component;

--component pll
    --port (CLKI: in  std_logic; CLKOP: out  std_logic; 
        --CLKOS: out  std_logic; CLKOS2: out  std_logic; 
        --CLKOS3: out  std_logic);
--end component;

--component i2c_slave
 --port (
---- generic ports
 --XRESET  : in  std_logic;                     -- System Reset
 --ready   : in  std_logic;                     -- back end system ready signal
 --start   : out std_logic;                     -- start of the i2c cycle
 --stop    : out std_logic;                     -- stop the i2c cycle
 --data_in : in  std_logic_vector(7 DOWNTO 0);  -- parallel data in
 --data_out: out std_logic_vector(7 DOWNTO 0);  -- parallel data out
 --r_w     : out std_logic;                     -- read/write signal to the reg_map bloc
 --data_vld: out std_logic;                     -- data valid from i2c
---- i2c ports
 --scl_in  : in std_logic;                      -- SCL clock line
 --scl_oe  : out std_logic;                     -- controls scl output enable
 --sda_in  : in std_logic;                      -- i2c serial data line in
 --sda_oe  : out std_logic                      -- controls sda output enable
 --);
--end component;

component ram_i2c
    port (DataInA: in  std_logic_vector(15 downto 0); 
        DataInB: in  std_logic_vector(7 downto 0); 
        AddressA: in  std_logic_vector(3 downto 0); 
        AddressB: in  std_logic_vector(4 downto 0); 
        ClockA: in  std_logic; ClockB: in  std_logic; 
        ClockEnA: in  std_logic; ClockEnB: in  std_logic; 
        WrA: in  std_logic; WrB: in  std_logic; ResetA: in  std_logic; 
        ResetB: in  std_logic; QA: out  std_logic_vector(15 downto 0); 
        QB: out  std_logic_vector(7 downto 0));
end component;

component fall_rise_detector  
    port ( 
             i_clk           : in std_logic ;
			 i_edge_select   : in std_logic ;   -- 0 - rising edge detector , 1 - falling edge detector 
			 i_signal        : in std_logic ;   -- input signal for edge detect 
			 o_edge          : out std_logic  
		  ); 
end component; 		  


--COMPONENTS End


begin

--I1: entity work.OSCG port map (OSC => s_OSC_internal);




PLL1 : entity work.pll
    port map (CLKI=>I_OSC_20MHZ, CLKOP=>s_OSC_100MHz1, CLKOS=>s_OSC_20MHz, CLKOS2=>s_OSC_100MHz2, CLKOS3=>s_OSC_100MHz3);


I2C1: entity work.i2c_slave
port map (
			-- generic ports
			XRESET => s_I2C_XRESET,
			ready => s_I2C_ready,
			start => s_I2C_start,
			stop => s_I2C_stop,
			data_in => s_I2C_data_in,
			data_out => s_I2C_data_out,
			r_w => s_I2C_r_w,
			data_vld => s_I2C_data_vld,
			-- i2c ports
			scl_in => I_RTC_SCL,
			scl_oe => s_I2C_scl_oe,
			sda_in => s_I2C_sda_read,
			sda_oe => s_I2C_sda_oe
);

--read EMIF | write I2C
RAM1 : ram_i2c
port map (	DataInA(15 downto 0)=>s_RAM_DataInA, 
			DataInB(7 downto 0)=>s_RAM_DataInB, 
			AddressA(3 downto 0)=>s_RAM_AddressA, 
			AddressB(4 downto 0)=>s_RAM_AddressB, 
			ClockA=>s_OSC_100MHz1,	
			ClockB=>s_OSC_20MHz, 
			ClockEnA=>'1', 
			--ClockEnB=>'1', 
			--ClockEnA=>s_RAM2_ResetA, 
			ClockEnB=>s_RAM2_ResetB, 	
			WrA=>'0', --WrA=>s_EMIF1_WE_FP, 
			WrB=>s_I2C_RAM_WE, 
			--ResetA=>s_RAM1_ResetA, ResetB=>s_RAM1_ResetB, 
			ResetA=>'0', ResetB=>'0', 
			QA(15 downto 0)=>s_RAM1_QA, 
			QB(7 downto 0)=>s_RAM1_QB);
		
--read EMIF | write I2C
RAM2 : ram_i2c
port map (	DataInA(15 downto 0)=>s_RAM_DataInA, 
			DataInB(7 downto 0)=>s_RAM_DataInB, 
			AddressA(3 downto 0)=>s_RAM_AddressA, 
			AddressB(4 downto 0)=>s_RAM_AddressB, 
			ClockA=>s_OSC_100MHz1,	
			ClockB=>s_OSC_20MHz, 
			ClockEnA=>'1', 
			--ClockEnB=>'1', 
			--ClockEnA=>s_RAM1_ResetA, 
			ClockEnB=>s_RAM1_ResetB, 
			WrA=>'0', --WrA=>s_EMIF1_WE_FP, 
			WrB=>s_I2C_RAM_WE, 
			--ResetA=>s_RAM2_ResetA, ResetB=>s_RAM2_ResetB, 
			ResetA=>'0', ResetB=>'0', 
			QA(15 downto 0)=>s_RAM2_QA, 
			QB(7 downto 0)=>s_RAM2_QB);
			
			
--read I2C | write EMIF
RAM3 : ram_i2c
port map (	DataInA(15 downto 0)=>s_RAM_DataInA, 
			DataInB(7 downto 0)=>s_RAM_DataInB, 
			AddressA(3 downto 0)=>s_RAM_AddressA, 
			AddressB(4 downto 0)=>s_RAM_AddressB, 
			ClockA=>s_OSC_100MHz1,	
			ClockB=>s_OSC_20MHz, 
			ClockEnA=>'1', 
			--ClockEnB=>'1', 
			--ClockEnA=>s_RAM4_ResetA, 
			ClockEnB=>s_RAM4_ResetB, 
			WrA=>s_EMIF1_WE_FP, 
			WrB=>'0', --WrB=>s_I2C_RAM_WE, 
			--ResetA=>s_RAM1_ResetA, ResetB=>s_RAM1_ResetB, 
			ResetA=>'0', ResetB=>'0', 
			QA(15 downto 0)=>s_RAM3_QA, 
			QB(7 downto 0)=>s_RAM3_QB);
		
--read I2C | write EMIF
RAM4 : ram_i2c
port map (	DataInA(15 downto 0)=>s_RAM_DataInA, 
			DataInB(7 downto 0)=>s_RAM_DataInB, 
			AddressA(3 downto 0)=>s_RAM_AddressA, 
			AddressB(4 downto 0)=>s_RAM_AddressB, 
			ClockA=>s_OSC_100MHz1,	
			ClockB=>s_OSC_20MHz, 
			ClockEnA=>'1', 
			--ClockEnB=>'1', 
			--ClockEnA=>s_RAM3_ResetA, 
			ClockEnB=>s_RAM3_ResetB, 
			WrA=>s_EMIF1_WE_FP, 
			WrB=>'0', --WrB=>s_I2C_RAM_WE, 
			--ResetA=>s_RAM2_ResetA, ResetB=>s_RAM2_ResetB, 
			ResetA=>'0', ResetB=>'0', 
			QA(15 downto 0)=>s_RAM4_QA, 
			QB(7 downto 0)=>s_RAM4_QB);
			
		
		
		
		-- EMIF EBR (EBR A Side)
		s_RAM_AddressA(3 downto 0) <= I_EMIF1_ADDRESS_FP(2) & I_EMIF1_ADDRESS_FP(1) & I_EMIF1_ADDRESS_FP(0) & I_EMIF1_EM1BA1_FP;
		s_EMIF1_WE_FP <= not I_EMIF1_nWE_FP when (I_EMIF1_nCS3_FP = '0') else '0';
		s_RAM_DataInA <= I_O_EMIF1_DATA_FP when (I_EMIF1_nCS3_FP = '0') and (I_EMIF1_nWE_FP = '0') else s_RAM_DataInA; -- writing data to EMIF EBR memory from DSP	
		-- reading data from EMIF EBR memory from DSP
		I_O_EMIF1_DATA_FP <= 	s_RAM1_QA when (s_RAM2_EmifRead = '0') and (I_EMIF1_nCS3_FP = '0') and ((I_EMIF1_nOE_FP = '0') or (s_EMIF_Read = '1')) else 
								s_RAM2_QA when (s_RAM2_EmifRead = '1') and (I_EMIF1_nCS3_FP = '0') and ((I_EMIF1_nOE_FP = '0') or (s_EMIF_Read = '1')) else 
								(OTHERS => 'Z'); 
		--s_RAM1_ResetA <= s_RAM2_EmifRead;
		--s_RAM2_ResetA <= not s_RAM2_EmifRead;
		
		-- Make Longer Pulse I_EMIF1_nOE_FP (with the size of I_EMIF1_nCS3_FP)
		s_EMIF_Read <= 	'0' when (I_EMIF1_nCS3_FP = '1') else
						'1' when (I_EMIF1_nCS3_FP = '0') and (I_EMIF1_nOE_FP = '0') else
						s_EMIF_Read;
		
		-- Change Buffer (RAM1/RAM2) when EMIF Read Address 0x003XXXX0
		s_RAM2_EmifRead_Previous <= s_RAM2_EmifRead when (I_EMIF1_nCS3_FP = '1') else s_RAM2_EmifRead_Previous;
		s_RAM2_EmifRead <= 	'1' when (s_RAM2_EmifRead_Previous = '0') and (I_EMIF1_ADDRESS_FP(2 downto 0) = "000") and (I_EMIF1_EM1BA1_FP = '0') and (I_EMIF1_nOE_FP = '0') and (I_EMIF1_nCS3_FP = '0') and (s_RAM2_EmifRead = '0')  and (s_I2C_RAM2_Valid = '1') else
							'0' when (s_RAM2_EmifRead_Previous = '1') and (I_EMIF1_ADDRESS_FP(2 downto 0) = "000") and (I_EMIF1_EM1BA1_FP = '0') and (I_EMIF1_nOE_FP = '0') and (I_EMIF1_nCS3_FP = '0') and (s_RAM2_EmifRead = '1')  and (s_I2C_RAM1_Valid = '1') else
							s_RAM2_EmifRead;



		-- Mark Buffer Busy (RAM3/RAM4) when EMIF Write Address 0x003XXXX0
		-- Change Buffer (RAM3/RAM4) when EMIF Write Address 0x003XXXX3
		process (s_OSC_100MHz2)
		begin
			if rising_edge(s_OSC_100MHz2) then
			
				if (s_I2C_RAM3_Busy = '1') then
					s_I2C_RAM3_Valid <= '0';
				end if;
				
				if (s_I2C_RAM4_Busy = '1') then
					s_I2C_RAM4_Valid <= '0';
				end if;

				if (I_EMIF1_nCS3_FP = '0') then
					if (I_EMIF1_nWE_FP = '0') then
						if (I_EMIF1_ADDRESS_FP(2 downto 0) = "000") and (I_EMIF1_EM1BA1_FP = '0') then
							if s_RAM4_EmifWrite = '1' then
								s_I2C_RAM4_Valid <= '0';
							else
								s_I2C_RAM3_Valid <= '0';
							end if;
							s_EmifWriteValidFlag <= '0';
						elsif (I_EMIF1_ADDRESS_FP(2 downto 0) = "001") and (I_EMIF1_EM1BA1_FP = '1') then
							s_EmifWriteValidFlag <= '1';
						else
							s_EmifWriteValidFlag <= '0';
						end if;
					end if;
				else
					if (s_EmifWriteValidFlag = '1') then
						if s_RAM4_EmifWrite = '1' then
							if (s_I2C_RAM3_Busy = '0') then
								s_RAM4_EmifWrite <= '0';
								s_EmifWriteValidFlag <= '0';
							end if;
							s_I2C_RAM3_Valid <= '0';
							s_I2C_RAM4_Valid <= '1';
						else
							if (s_I2C_RAM4_Busy = '0') then
								s_RAM4_EmifWrite <= '1';
								s_EmifWriteValidFlag <= '0';
							end if;
							s_I2C_RAM3_Valid <= '1';
							s_I2C_RAM4_Valid <= '0';
						end if;
					end if;
				end if;
				
			end if;
		end process;

		

		
		-- I2C EBR (EBR B Side)
		s_I2C_wr_addr_vector <= std_logic_vector(to_unsigned(s_I2C_wr_addr_memory));
		s_I2C_rd_addr_vector <= std_logic_vector(to_unsigned(s_I2C_rd_addr_memory));
		s_RAM_AddressB(4 downto 0) <= s_I2C_wr_addr_vector when s_I2C_r_w = '0' else s_I2C_rd_addr_vector;
		s_I2C_RAM_WE <= '1' when s_I2C_r_w = '0' else '0';
		s_RAM_DataInB <= s_I2C_data_out;
		
		--s_I2C_data_in <= s_RAM1_QB when s_RAM2_EmifRead = '1' else s_RAM2_QB;
		s_RAM1_ResetB <= not s_RAM2_EmifRead;
		s_RAM2_ResetB <= s_RAM2_EmifRead;
		
		s_I2C_data_in <= s_RAM3_QB when s_RAM4_EmifWrite = '1' else s_RAM4_QB;
		s_RAM3_ResetB <= not s_RAM4_EmifWrite;
		s_RAM4_ResetB <= s_RAM4_EmifWrite;
		
		
		-- Debug
		O_GPIO_FPGA_1V8(0) <= s_I2C_r_w;
		O_GPIO_FPGA_1V8(1) <= s_EmifWriteValidFlag;
		O_GPIO_FPGA_1V8(2) <= I_EMIF1_nCS3_FP;
		O_GPIO_FPGA_1V8(3) <= I_EMIF1_nWE_FP;
		O_GPIO_FPGA_1V8(4) <= s_I2C_stop;
		O_GPIO_FPGA_1V8(5) <= s_I2C_sda_oe_en;
		 
		-- I2C_Slave Module Interface
		I_O_RTC_SDA <= '0' when (s_I2C_sda_oe = '1') and (s_I2C_sda_oe_en = '1') else 'Z';
		s_I2C_sda_read <= I_O_RTC_SDA;
		s_I2C_ready <= '1';		--if clock streching needed set to 0
		
		-- I2C_Slave Module Reset
		P_Clock_DIV_I2C : process(s_OSC_20MHz)
		begin
			if rising_edge(s_OSC_20MHz) then
				s_I2C_XRESET_VECTOR(0 to 3) <= s_I2C_XRESET_VECTOR(1) & s_I2C_XRESET_VECTOR(2) & s_I2C_XRESET_VECTOR(3) & '0'; 
				s_I2C_XRESET <= s_I2C_XRESET_VECTOR(0);
			end if;
		end process;

				
		-- I2C Edge Detectors 
		EDGE1 : fall_rise_detector  
		port map  ( 
             i_clk => s_OSC_20MHz,
			 i_edge_select => '0',   		
			 i_signal => s_I2C_data_vld, 
			 o_edge => s_I2C_data_vld_detect
		  ); 
		  
		EDGE2 : fall_rise_detector  
		port map  ( 
             i_clk => s_OSC_20MHz,
			 i_edge_select => '0',   		
			 i_signal => s_I2C_start, 
			 o_edge => s_I2C_start_detect
		  ); 
		  
		EDGE3 : fall_rise_detector  
		port map  ( 
             i_clk => s_OSC_20MHz,
			 i_edge_select => '0',   		
			 i_signal => s_I2C_sda_oe, 
			 o_edge => s_I2C_sda_oe_detect
		  ); 
		  
		EDGE4 : fall_rise_detector  
		port map  ( 
             i_clk => s_OSC_20MHz,
			 i_edge_select => '0',   		
			 i_signal => s_I2C_stop, 
			 o_edge => s_I2C_stop_detect
		  ); 
		  
		-- I2C Module IO Process  
		process (s_OSC_20MHz) 
		begin
			if falling_edge(s_OSC_20MHz) then				
				case s_I2C_state is
					when idle =>
												
						if s_I2C_sda_oe_detect = '1' then
						
							if s_I2C_r_w = '0' then	--I2C write request
							
								if s_RAM2_EmifRead = '0' then
									s_I2C_RAM2_Valid <= '0';
								else
									s_I2C_RAM1_Valid <= '0';
								end if;
								s_I2C_wr_addr <= 0;
								s_I2C_wr_addr_memory <= 0;
								--s_I2C_ready <= '1';						--Has Always Data for I2C Write Command
								s_I2C_state <= start;
								s_I2C_sda_oe_en <= '1';
								
							else										--I2C read request
							
								if (s_RAM4_EmifWrite = '1') then
									if (s_I2C_RAM3_Valid = '1') then
										s_I2C_RAM3_Busy <= '1';
										--s_I2C_ready <= '1';				--Has Data for I2C Read Command
										s_I2C_state <= start;
										s_I2C_rd_addr_memory <= 0;
										s_I2C_sda_oe_en <= '1';
									else
										--s_I2C_ready <= '0';	
										s_I2C_state <= completed;										
									end if;
								else
									if (s_I2C_RAM4_Valid = '1') then
										s_I2C_RAM4_Busy <= '1';
										--s_I2C_ready <= '1';				--Has Data for I2C Read Command
										s_I2C_state <= start;
										s_I2C_rd_addr_memory <= 0;
										s_I2C_sda_oe_en <= '1';
									else
										--s_I2C_ready <= '0';	
										s_I2C_state <= completed;
									end if;
								end if;
								
							end if;
						
						else
							s_I2C_sda_oe_en <= '0';
							--s_I2C_ready <= '1';	
							s_I2C_state <= idle;
						end if;
					
					when start =>
						if s_I2C_stop_detect = '1' then
							
							s_I2C_state <= completed;
							if s_I2C_r_w = '0' then	--I2C write request
								if s_RAM2_EmifRead = '0' then
									s_I2C_RAM2_Valid <= '1';
									s_I2C_RAM1_Valid <= '0';
								else
									s_I2C_RAM1_Valid <= '1';
									s_I2C_RAM2_Valid <= '0';
								end if;
							s_RAM2_EmifRead_Last <= s_RAM2_EmifRead;
							else
							end if;
						elsif s_I2C_data_vld_detect = '1' then
							if s_I2C_r_w = '0' then	--I2C write request
								s_I2C_wr_addr <= s_I2C_wr_addr + 1;
								s_I2C_wr_addr_memory <= s_I2C_wr_addr;
							else	--I2C read request
								s_I2C_rd_addr_memory <= s_I2C_rd_addr_memory + 1;
							end if;
							s_I2C_state <= start;
						else
							s_I2C_state <= start;
						end if;
					
					when completed =>
						s_I2C_RAM3_Busy <= '0';
						s_I2C_RAM4_Busy <= '0';
						if s_RAM2_EmifRead_Last = '1' then
							if s_RAM2_EmifRead = '0' then
								s_I2C_RAM1_Valid <= '0';
								s_I2C_RAM2_Valid <= '0';
							end if;
						else
							if s_RAM2_EmifRead = '1' then
								s_I2C_RAM1_Valid <= '0';
								s_I2C_RAM2_Valid <= '0';
							end if;
						end if;
						
						s_I2C_sda_oe_en <= '0';
						
						if s_I2C_start_detect = '1' then
							s_I2C_state <= idle;
							--if s_RAM2_EmifRead = '0' then
								--if s_I2C_RAM2_Valid = '0' then
									--s_I2C_state <= idle;
								--else
									--s_I2C_state <= completed;
								--end if;
							--else
								--if s_I2C_RAM1_Valid = '0' then
									--s_I2C_state <= idle;
								--else
									--s_I2C_state <= completed;
								--end if;
							--end if;
							
						else
							s_I2C_state <= completed;
						end if;
						
					when others =>
						s_I2C_state <= completed;  -- default state	
						
				end case;		
			end if;
		end process;
		 
	




		-- PWM signals passing-through
		O_PWM_1_U_TOP <= I_EPWM_1_P;
		O_PWM_1_U_BOT <= I_EPWM_1_N;
		O_PWM_1_V_TOP <= I_EPWM_2_P;
		O_PWM_1_V_BOT <= I_EPWM_2_N;
		O_PWM_1_W_TOP <= I_EPWM_3_P;
		O_PWM_1_W_BOT <= I_EPWM_3_N;
		O_PWM_2_U_TOP <= I_EPWM_4_P;
		O_PWM_2_U_BOT <= I_EPWM_4_N;
		O_PWM_2_V_TOP <= I_EPWM_5_P;
		O_PWM_2_V_BOT <= I_EPWM_5_N;
		O_PWM_2_W_TOP <= I_EPWM_6_P;
		O_PWM_2_W_BOT <= I_EPWM_6_N;
		O_PWM_3_U_TOP <= I_EPWM_7_P;
		O_PWM_3_U_BOT <= I_EPWM_7_N;
		O_PWM_3_V_TOP <= I_EPWM_8_P;
		O_PWM_3_V_BOT <= I_EPWM_8_N;
		O_PWM_3_W_TOP <= I_EPWM_9_P;
		O_PWM_3_W_BOT <= I_EPWM_9_N;
		O_PWM_4_U_TOP <= I_EPWM_10_P;
		O_PWM_4_U_BOT <= I_EPWM_10_N;
		O_PWM_4_V_TOP <= I_EPWM_11_P;
		O_PWM_4_V_BOT <= I_EPWM_11_N;
		O_PWM_4_W_TOP <= I_EPWM_12_P;
		O_PWM_4_W_BOT <= I_EPWM_12_N;	
		
		I_O_DSP_EQEP1A_FPGA <= I_O_DSP_EQEP2A_FPGA;
		I_O_DSP_EQEP1B_FPGA <= I_O_DSP_EQEP2B_FPGA;
		I_O_DSP_EQEP1S_FPGA <= I_O_DSP_EQEP2S_FPGA;
		I_O_DSP_EQEP1I_FPGA <= I_O_DSP_EQEP2I_FPGA;
		I_O_DSP_SPI_C_SIMO_FPGA <= I_O_DSP_SPI_C_SOMI_FPGA;
		O_DSP_SCI_D_RX_FPGA <= I_DSP_SCI_D_TX_FPGA;
		I_O_WROOM_SPI_SDI <= I_O_WROOM_SPI_SDO or I_O_WROOM_SPI_CS or I_O_WROOM_SPI_CLK;
		
		O_WROOM_U0RXD <= I_WROOM_U0RXD_FPGA;
		O_WROOM_U0TXD_FPGA <= I_WROOM_U0TXD;
		
		O_WROOM_EN <= 'Z';
		O_WROOM_GPIO0 <= 'Z';
		
		O_PIC16_UART_RX <= I_PIC16_UART_TX;
		
	
		I_O_PWM_BUF <= (others => 'Z');
		I_O_BM_SEL0_SCITXDA <= 'Z';

		I_O_EMIF1_DATA_FP <= (others => 'Z');

		O_ANYBUS_nRESET <= I_ANYBUS_nIRQ
						or I_DSP_ERROR_RESET_FPGA;

		I_O_FLT <= (others => 'Z');

		O_DSP_RESET <= 'Z';
		O_PROGRAMN <= 'Z';

		I_O_RTC_nRESET <= 'Z';

		O_FPGA_EEPROM_CLK <= 'Z';
		O_FPGA_EEPROM_SIMO <= I_FPGA_EEPROM_SOMI;
		O_FPGA_EEPROM_nCS <= 'Z';
		O_MCLK <= 'Z';
		I_O_DSP_SPI_C_TE0_FPGA <= 'Z';
		I_O_DSP_SPI_C_CLK_FPGA <= 'Z';
	
end rtl;