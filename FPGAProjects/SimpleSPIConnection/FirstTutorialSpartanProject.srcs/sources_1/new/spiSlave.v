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
    input sys_clk,
    input FPGA_QSPI_CLK,
    input FPGA_QSPI_CS,
    input FPGA_QSPI_D,
    output FPGA_QSPI_Q,
    output reg led2
    );
    
reg [8:0] shift_r;
assign FPGA_QSPI_Q = shift_r[8];

initial
    led2 <= 0;

// Sync Clocks
reg sclk;
reg scs;
reg smosi;
reg oldcs;
reg oldclk;

always @(posedge sys_clk) begin
    oldclk <= sclk;
    sclk <= FPGA_QSPI_CLK;
end

always @(posedge sys_clk) begin
    oldcs <= scs;
    scs <= FPGA_QSPI_CS;
end

always @(posedge sys_clk) begin
    smosi = FPGA_QSPI_D;
end

initial
    oldcs <= 1;

    
always @(posedge sys_clk) begin
    if (!oldcs & scs) begin
        shift_r <= shift_r*3;
    end else begin 
        if (!oldclk & sclk) begin
            shift_r <= shift_r << 1;
            shift_r[0] <= FPGA_QSPI_D;
        end
    end
end


  
    
endmodule
