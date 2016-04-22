-- template testbench 
LIBRARY IEEE;
USE IEEE.std_logic_1164.ALL;
 
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--USE IEEE.numeric_std.ALL;
 
-- change name for a proper declaration
ENTITY tb_controller IS
END tb_controller;

--                       change name  
ARCHITECTURE behavior OF tb_controller IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
    -- declare your IP-Core which you want to test
    COMPONENT controller
    PORT(
         clk : in STD_LOGIC;
         reset : in STD_LOGIC;
         switches_in : in STD_LOGIC_VECTOR (7 downto 0);
         buttons_in : in STD_LOGIC_VECTOR (4 downto 0);
         leds_out : out STD_LOGIC_VECTOR (7 downto 0)
        );
    END COMPONENT;
    
    
    -- declare all input and output signals which appears inside the testbench 
    --Inputs
    signal clk : std_logic := '0';
    signal reset : std_logic := '0';
    signal switches_in : STD_LOGIC_VECTOR (7 downto 0);
    signal buttons_in : STD_LOGIC_VECTOR (4 downto 0);

    --Outputs
    signal leds_out : STD_LOGIC_VECTOR (7 downto 0);


    ------------------------------
    -- Clock period definitions 
    constant clk_period : time := 10 ns; --(100MHz)
    ------------------------------
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
	-- map the ports of your IP-Core which you want to test, with the signals decleared above
    uut: controller PORT MAP (
          clk,
          reset,
          switches_in,
          buttons_in,
          leds_out
    );

    ------------------------------
    -- Clock process definitions
    clk_process :process
    begin
		clk <= '0';
		wait for clk_period/2;
		clk <= '1';
		wait for clk_period/2;
    end process;
    ------------------------------

    -- Stimulus process 
    stim_proc: process
    begin		
        buttons_in  <= (others => '0');
        switches_in  <= "00000101";
        
        -- resets the UUT  
        -- hold reset state for 100 ns.
        reset <= '0';
        wait for 100 ns;	
        reset <= '1';
        
        -- insert your testcode here
        wait for 100 ns;
        switches_in  <= "11110101";
        
        wait for 1000 ns;
        switches_in  <= "00000010";
        
        -- stop the controller
        wait for 500 ns;
        buttons_in  <= "00001";
        wait for 500 ns;
        buttons_in  <= "00000";
        
        -- restart the controller
        wait for 100 ns;
        buttons_in  <= "00001";    
        wait for 100 ns;
        buttons_in  <= "00000";
      
        wait for 100 ns;
        buttons_in  <= "00100";
        wait for 100 ns;
        buttons_in  <= "00100";
        wait for 100 ns;
        buttons_in  <= "00100";
        wait for 100 ns;
        buttons_in  <= "00100";
        wait for 100 ns;
        buttons_in  <= "00100";
        wait for 100 ns;
        
        wait for clk_period*10;
        wait;
    end process;

END;
