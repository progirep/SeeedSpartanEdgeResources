`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 12/28/2020 07:01:12 PM
// Design Name: 
// Module Name: toplevel
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module toplevel(
        input sys_clk,
        input [1:0]key,
        input FPGA_QSPI_CLK,
        input FPGA_QSPI_CS,
        input FPGA_QSPI_D,
        output FPGA_QSPI_Q,
        output led1,
        output led2
        
    );

wire wasted;
    
test test_instance(
        .sys_clk(sys_clk), .key(key), .led1(led1), .led2(wasted) ); 
   
 

spiSlave spi_instance(
    .FPGA_QSPI_CLK(FPGA_QSPI_CLK),
    .FPGA_QSPI_CS(FPGA_QSPI_CS),
    .FPGA_QSPI_D(FPGA_QSPI_D),
    .FPGA_QSPI_Q(FPGA_QSPI_Q),
    .led2(led2)
);       
    
    
endmodule


