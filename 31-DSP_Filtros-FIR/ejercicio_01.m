
clc
clear 
close all

SNR = 15; % dB
N = 10;

FN = 100;
FS = 10000;
t=0:1/FS:0.5;

y =  (sin(2*pi * FN * t))';

y_n = my_noise (y, SNR);

B = ones(1,N) ./ N;
A = 1;
y_ma = filter(B, A, y_n);

freqz(B,A,256)

figure
plot(t, y, 'b')
hold on
plot(t, y_n, 'g')
hold on
plot(t, y_ma, 'r')

[~, f, ~, ~, y_mag, ~, ~] = my_dft(y, FS);
[~, f_ma, ~, ~, y_mag_ma, ~, ~] = my_dft(y_ma, FS);

figure
plot(f, y_mag, 'b')
hold on
plot(f_ma, y_mag_ma, 'r')

