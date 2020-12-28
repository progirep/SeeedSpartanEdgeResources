`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2019/08/05 11:02:11
// Design Name: 
// Module Name: test
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


module test(
        input sys_clk,
        input [1:0]key,
        output reg led1,
        output reg led2
    );


wire flag = key[0] & key[1];
always @(posedge sys_clk)begin
    if(flag)
      begin
        led1 <= 1'b1;
        led2 <= 1'b1;
      end
    else
      begin
        led1 <= 1'b0;
        led2 <= 1'b0;
      end
end
endmodule