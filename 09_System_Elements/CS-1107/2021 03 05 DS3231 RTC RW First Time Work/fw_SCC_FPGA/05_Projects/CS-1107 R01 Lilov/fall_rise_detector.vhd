library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity fall_rise_detector is 
     Port ( 
             i_clk           : in std_logic ;
			 i_edge_select   : in std_logic ;   -- 0 - rising edge detector , 1 - falling edge detector 
			 i_signal        : in std_logic ;   -- input signal for edge detect 
			 o_edge          : out std_logic  
		  ); 
     end fall_rise_detector ; 		  
	 
architecture Behavioral of fall_rise_detector is

signal s_reg1   : std_logic := '0' ; 
signal s_reg2   : std_logic := '0' ; 
signal s_signal : std_logic := '0' ;
signal s_edge   : std_logic := '0' ; 


begin 

o_edge   <= s_edge ;
s_signal <= i_signal ; 

 process (i_clk) 
    begin 
	   if rising_edge (i_clk) then 
	      s_reg1 <= i_signal ; 
		  s_reg2 <= s_reg1 ; 
	   end if ; 
	end process ;

s_edge <= s_reg1 and (not s_reg2) when  i_edge_select = '0' else  -- rising edge detect 
          s_reg2 and (not s_reg1);                                -- falling edge 
																  
																  
end Behavioral;