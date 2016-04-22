----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 13.04.2016 16:12:52
-- Design Name: 
-- Module Name: timer - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.numeric_std.all;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity timer is
    Port ( clk : in STD_LOGIC;
           reset : in STD_LOGIC;
           enable : in STD_LOGIC;
           target : in STD_LOGIC_VECTOR (31 downto 0);
           irq : out STD_LOGIC);
end timer;

architecture Behavioral of timer is
signal timer: std_logic_vector(31 downto 0);
begin

    process(clk)
        variable timer_temp : std_logic_vector (31 downto 0);
    begin
        if rising_edge(clk) then
            irq <= '0';
            if reset = '0' then
                timer <= (others => '0');
            else
                if enable ='1' then
                    timer_temp := std_logic_vector( unsigned(timer) + 1); 
                    if timer_temp >= target then
                        timer <= (others => '0');
                        irq <= '1';
                    else
                        timer <= timer_temp;
                    end if;
                end if;
            end if;
        end if;
    end process;
    
end Behavioral;