-- template testbench 
LIBRARY IEEE;
USE IEEE.std_logic_1164.ALL;
 
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--USE IEEE.numeric_std.ALL;
 
-- change name for a proper declaration
ENTITY tb_timer IS
END tb_timer;

--                       change name  
ARCHITECTURE behavior OF tb_timer IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
    -- declare your IP-Core which you want to test
    COMPONENT timer
    PORT(
         clk : in STD_LOGIC;
               reset : in STD_LOGIC;
               enable : in STD_LOGIC;
               target : in STD_LOGIC_VECTOR (15 downto 0);
               irq : out STD_LOGIC
        );
    END COMPONENT;
    
    
    -- declare all input and output signals which appears inside the testbench 
    --Inputs
    signal clk : std_logic := '0';
    signal reset : std_logic := '0';
    signal enable : std_logic := '0';
    signal target : std_logic_vector(15 downto 0) := "0000000000000000";

    --Outputs
    signal output : std_logic;


    ------------------------------
    -- Clock period definitions 
    constant clk_period : time := 10 ns; --(100MHz)
    ------------------------------
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
	-- map the ports of your IP-Core which you want to test, with the signals decleared above
    uut: timer PORT MAP (
          clk,
          reset,
          enable,
          target,
          output
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
      
      -- resets the UUT  
      -- hold reset state for 100 ns.
	  reset <= '0';
	  target <= "0000000000000100";
	  enable <= '1';
      wait for 100 ns;	
	  reset <= '1';
	  wait for 100 ns;
	  enable <= '0';
	  wait for 100 ns;
	  reset <= '0';
	  
	  -- insert your testcode here	
		
	  wait for clk_period*10; 	
      wait;
    end process;

END;
