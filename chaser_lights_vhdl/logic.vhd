----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 13.04.2016 16:10:08
-- Design Name: 
-- Module Name: logic - Behavioral
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
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity controller is
    Port ( clk : in STD_LOGIC;
           reset : in STD_LOGIC;
           switches_in : in STD_LOGIC_VECTOR (7 downto 0);
           buttons_in : in STD_LOGIC_VECTOR (4 downto 0);
           leds_out : out STD_LOGIC_VECTOR (7 downto 0));
end controller;

architecture Behavioral of controller is

    COMPONENT timer
    PORT(
         clk : in STD_LOGIC;
         reset : in STD_LOGIC;
         enable : in STD_LOGIC;
         target : in STD_LOGIC_VECTOR (31 downto 0);
         irq : out STD_LOGIC
        );
    END COMPONENT;

    constant LED_0 : std_logic_vector (7 downto 0) := "00000001";
    constant LED_1 : std_logic_vector (7 downto 0) := "00000010"; 
    constant LED_2 : std_logic_vector (7 downto 0) := "00000100"; 
    constant LED_3 : std_logic_vector (7 downto 0) := "00001000"; 
    constant LED_4 : std_logic_vector (7 downto 0) := "00010000"; 
    constant LED_5 : std_logic_vector (7 downto 0) := "00100000"; 
    constant LED_6 : std_logic_vector (7 downto 0) := "01000000";
    constant LED_7 : std_logic_vector (7 downto 0) := "10000000";

    constant BTNC : std_logic_vector (4 downto 0) := "00001";
    constant BTND : std_logic_vector (4 downto 0) := "00010";
    constant BTNL : std_logic_vector (4 downto 0) := "00100";
    constant BTNR : std_logic_vector (4 downto 0) := "01000";
    constant BTNU : std_logic_vector (4 downto 0) := "10000";

    constant SW_0 : std_logic_vector (7 downto 0) := "00000001";
    constant SW_1 : std_logic_vector (7 downto 0) := "00000010"; 
    constant SW_2 : std_logic_vector (7 downto 0) := "00000100"; 
    constant SW_3 : std_logic_vector (7 downto 0) := "00001000"; 
    constant SW_4 : std_logic_vector (7 downto 0) := "00010000"; 
    constant SW_5 : std_logic_vector (7 downto 0) := "00100000"; 
    constant SW_6 : std_logic_vector (7 downto 0) := "01000000";
    constant SW_7 : std_logic_vector (7 downto 0) := "10000000";
    
    constant clk_per_sec_8 : integer := 800000000;
    constant clk_per_sec_7 : integer := 700000000;
    constant clk_per_sec_6 : integer := 600000000;
    constant clk_per_sec_5 : integer := 500000000;
    constant clk_per_sec_4 : integer := 400000000;
    constant clk_per_sec_3 : integer := 300000000;
    constant clk_per_sec_2 : integer := 200000000;
    constant clk_per_sec_1 : integer := 100000000;

    signal timer_output : std_logic;
    signal timer_output_last : std_logic;
    signal direction_right : std_logic;
    signal leds_intern : std_logic_vector (7 downto 0);
    signal enable : std_logic;
    signal timer_target : std_logic_vector (31 downto 0);
    signal seconds : unsigned (7 downto 0);
    signal BTNC_last : std_logic;

begin
    timer_instance: timer PORT MAP (
          clk,
          reset,
          enable,
          timer_target,
          timer_output
    );

    leds_out <= leds_intern;

    clock_job: process(clk, reset, leds_intern, switches_in, buttons_in, timer_output, direction_right, enable)
        variable target_temp : unsigned (31 downto 0);
    begin
        if rising_edge(clk) then        
            if reset = '0' then
                -- reset
                timer_target <= (others => '0');
                leds_intern <= "10000000";
                enable <= '1';
                BTNC_last <= '0';
                timer_output_last <= '0';
                direction_right <= '1';
            else
                if BTNC_last /= buttons_in(0) then
                    if buttons_in(0) = '1' then
                        enable <= not enable;
                    end if;
                    BTNC_last <= buttons_in(0);
                end if;
                
                if buttons_in(2) = '1' and leds_intern /= LED_7 then
                    -- set direction left
                    direction_right <= '0';
                elsif buttons_in(3) = '1' and leds_intern /= LED_0 then
                    --set direction right
                    direction_right <= '1';
                end if;
                
                if direction_right = '1' and leds_intern = LED_0 then
                    direction_right <= '0';
                elsif direction_right = '0' and leds_intern = LED_7 then
                    direction_right <= '1';
                end if;
                
                if timer_output /= timer_output_last and enable = '1' then
                    timer_output_last <= timer_output;
                    if timer_output = '1' then
                        if direction_right = '0' and buttons_in(3) /= '1' then
                            leds_intern <= std_logic_vector(unsigned(leds_intern) sll 1);
                        elsif direction_right = '1' and buttons_in(2) /= '1' then
                            leds_intern <= std_logic_vector(unsigned(leds_intern) srl 1);
                        end if;
                    end if;
                end if;
            
                -- run                
                target_temp := (others => '0');
                if switches_in(7) = '1' then
                    target_temp := target_temp + clk_per_sec_8;
                end if;
                if switches_in(6) = '1' then
                    target_temp := target_temp + clk_per_sec_7;
                end if;
                if switches_in(5) = '1' then
                    target_temp := target_temp + clk_per_sec_6;
                end if;
                if switches_in(4) = '1' then
                    target_temp := target_temp + clk_per_sec_5;
                end if;
                if switches_in(3) = '1' then
                    target_temp := target_temp + clk_per_sec_4;
                end if;
                if switches_in(2) = '1' then
                    target_temp := target_temp + clk_per_sec_3;
                end if;
                if switches_in(1) = '1' then
                    target_temp := target_temp + clk_per_sec_2;
                end if;
                if switches_in(0) = '1' then
                    target_temp := target_temp + clk_per_sec_1;
                end if;
                timer_target <= std_logic_vector(target_temp);
            end if;
        end if;
    end process;
end Behavioral;
