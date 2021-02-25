## Timing Assertions Section
# Primary clocks
# Virtual clocks
# Generated clocks
# Clock Groups
# Bus Skew constraints
# Input and output delay constraints

## Timing Exceptions Section
# False Paths
# Max Delay / Min Delay
# Multicycle Paths
# Case Analysis
# Disable Timing

## Physical Constraints Section
# located anywhere in the file, preferably before or after the timing constraints# or stored in a separate constraint file

set_property IOSTANDARD LVCMOS33 [get_ports {key[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports {key[1]}]
set_property IOSTANDARD LVCMOS33 [get_ports led1]
set_property IOSTANDARD LVCMOS33 [get_ports led2]
set_property IOSTANDARD LVCMOS33 [get_ports sys_clk]
set_property PACKAGE_PIN A13 [get_ports led2]
set_property PACKAGE_PIN J1 [get_ports led1]
set_property PACKAGE_PIN H4 [get_ports sys_clk]
set_property PACKAGE_PIN C3 [get_ports {key[0]}]
set_property PACKAGE_PIN M4 [get_ports {key[1]}]


# SPI Interface between ESP32 and FPGA
set_property PACKAGE_PIN P2 [get_ports FPGA_QSPI_D]
set_property PACKAGE_PIN H14 [get_ports FPGA_QSPI_CLK]
set_property PACKAGE_PIN M13 [get_ports FPGA_QSPI_CS]
set_property PACKAGE_PIN L14 [get_ports FPGA_QSPI_Q]
set_property PACKAGE_PIN D13 [get_ports FPGA_QSPI_HD]
set_property PACKAGE_PIN J13 [get_ports FPGA_QSPI_WP]
set_property IOSTANDARD LVCMOS33 [get_ports FPGA_QSPI_D]
set_property IOSTANDARD LVCMOS33 [get_ports FPGA_QSPI_CLK]
set_property IOSTANDARD LVCMOS33 [get_ports FPGA_QSPI_CS]
set_property IOSTANDARD LVCMOS33 [get_ports FPGA_QSPI_Q]
set_property IOSTANDARD LVCMOS33 [get_ports FPGA_QSPI_HD]
set_property IOSTANDARD LVCMOS33 [get_ports FPGA_QSPI_WP]
set_property SLEW FAST [get_ports FPGA_QSPI_Q]

set_property CLOCK_DEDICATED_ROUTE FALSE [get_nets {FPGA_QSPI_CLK_IBUF}]
# set_property CLOCK_DEDICATED_ROUTE FALSE [get_nets {led2_OBUF}]

