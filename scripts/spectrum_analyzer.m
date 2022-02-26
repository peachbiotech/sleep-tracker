[freq] = function SpectrumAnalyzer(real_vals, f_s)

freqs = [10, 50, 100];
f_s = 420;
t_f = 100;
timestamps = linspace(0,t_f,f_s*t_f);
sig = sin(2*pi*freqs(1)*timestamps)+sin(2*pi*freqs(2)*timestamps)+sin(2*pi*freqs(3)*timestamps);
plot(sig(1:1000))
fft(sig);
ans = abs(ans);
sig = sig+2*randn(size(timestamps));
plot(sig(1:1000))
L = t_f*f_s;
Y = fft(sig);
P2 = abs(Y/L);
P1 = P2(1:L/2+1);
P1(2:end-1) = 2*P1(2:end-1);
f = f_s*(0:(L/2))/L;
plot(f,P1) 
title('Single-Sided Amplitude Spectrum of X(t)')
xlabel('f (Hz)')
ylabel('|P1(f)|')


end
