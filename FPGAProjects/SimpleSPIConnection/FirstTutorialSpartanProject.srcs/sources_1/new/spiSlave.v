`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 12/28/2020 03:42:24 PM
// Design Name: 
// Module Name: spiSlave
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


module spiSlave(
    input FPGA_QSPI_CLK,
    input FPGA_QSPI_CS,
    input FPGA_QSPI_D,
    output FPGA_QSPI_Q,
    output reg led2
    );
    
reg [8:0] shift_r;
assign FPGA_QSPI_Q = shift_r[8];
// assign led2 = FPGA_QSPI_CS  ^ FPGA_QSPI_CLK ^ FPGA_QSPI_D;

initial
    led2 <= 0;
    
always @(posedge FPGA_QSPI_CLK) begin
    led2 <= 1;
    if (!FPGA_QSPI_CS) begin
        shift_r <= shift_r << 1;
        shift_r[0] <= FPGA_QSPI_D;
        // led2 <= shift_r[3];
    end
end
  
    
endmodule
