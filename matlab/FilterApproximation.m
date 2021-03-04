clc
close all
clear variables

set(0, 'DefaultLineLineWidth', 2);

%% Approximation of filter parameters
mData = csvread('sweeptest.csv', 1);
  vF = mData(:, 1);
  vMag = mData(:, 2);
  vMagDb = mData(:, 3);

bFLims = (vF >= 20) & (vF <= 20e3);
  vF = vF(bFLims);
  vMag = vMag(bFLims);
  vMagDb = vMagDb(bFLims);

mTrue = [150, +5, 1; 900, -7, 1; 3e3, +6, 1]; % true filter settings: f, magnitude in dB, bw in oct

% curve fit by polynomial regression
nFiltersMax = 4; % max number of filters

% Initial guess
[iFc, vMagC] = peakfinder(abs(vMagDb));
nFilters = length(iFc);
if nFilters > nFiltersMax % select 4 largest peaks
  [~, perm] = sort(vMagC, 'descend');
  iFc = sort(iFc(perm(1:4)), 'ascend');
end
vFc = vF(iFc);
  vAc = vMagDb(iFc); % amplitude initialization
  vQc = ones(size(vAc)); % quality initialization

figure('Units', 'Centimeters', 'Position', [52, 2, 24, 15]);
  cLegend = cell(length(vFc) + 3, 1);
    cLegend{1} = 'Gemessen'; cLegend{2} = 'Initialschätzung';
  semilogx(vF, vMagDb, 'x');
  hold on; plot(vFc, vAc, 'o', 'MarkerSize', 30)
  vHInitial = zeros(size(vMagDb));
  for iC = 1:length(vFc)
    [~, vH] = func_peqtf(10^(vAc(iC) / 20), 2 * pi * vFc(iC), sqrt(2) * vQc(iC), 1i * 2 * pi * vF);
      vHInitial = vHInitial + vH;
    semilogx(vF, vH, '--');
    cLegend{iC + 2} = ['Glocke ', num2str(iC)];
  end
  cLegend{iC + 3} = 'Initialschätzung';
  semilogx(vF, vHInitial);
    text(200, vAc(1), ['f_c=', num2str(vFc(1)), 'Hz, A=', num2str(vAc(1)), 'dB']);
    text(1100, vAc(2), ['f_c=', num2str(vFc(2)), 'Hz, A=', num2str(vAc(2)), 'dB']);
    text(4000, vAc(3), ['f_c=', num2str(vFc(3)), 'Hz, A=', num2str(vAc(3)), 'dB']);
  xlim([20 20e3]), ylim([-7, 7]), grid on;
  xlabel('Frequenz in Hz'); ylabel('Amplitude in dB');
  legend(cLegend, 'Location', 'southwest');

% Parameter optimatzion
vS = 0 + 1i * (2 * pi * vF);

vB0 = [vFc', vAc', vQc'];

hMagFit = @(vB0) func_MagFit(vS, vB0);

J = @(vP) sum((vMagDb - hMagFit(vP)).^2);

% options = optimoptions('fmincon', 'Display', 'iter');
vLb = [vFc' * 0.99, vAc' - 2, 0.1, 0.1, 0.1];
vUb = [vFc' * 1.01, vAc' + 2, 2, 2, 2];
[vB, fval, exitflag, output, lambda, grad] = fmincon(J, vB0, [], [], [], [], vLb, vUb);

vFc = vB(1:3);
vAc = vB(4:6);
vQc = vB(7:9);

fprintf('\n');
for iF = 1:length(vFc)
  fprintf(['\tf', num2str(iF), ' = ', num2str(vFc(iF)), 'Hz; e(f', num2str(iF), ') = ', num2str(mTrue(iF, 1) - vFc(iF)), 'Hz\n' ]);
  fprintf(['\tA', num2str(iF), ' = ', num2str(vAc(iF)), 'dB; e(A', num2str(iF), ') = ', num2str(mTrue(iF, 2) - vAc(iF)), 'dB\n' ]);
  fprintf(['\tQ', num2str(iF), ' = ', num2str(vQc(iF)), '; e(Q', num2str(iF), ') = ', num2str(mTrue(iF, 3) - vQc(iF)), '\n\n' ]);
end

% compute TF with estimated data
mH = zeros(length(vS), size(mTrue, 1));
for iE = 1:size(mTrue, 1)
  sW0 = 2 * pi * vFc(iE);
  sA0 = 10^(vAc(iE) / 20);
  sQp = sqrt(2) * vQc(iE); % WHY * sqrt(2)?!
  
  mH(:, iE) = func_peqtf(sA0, sW0, sQp, vS);
end
vHdB = 20 * log10(abs(prod(mH, 2)));

sMSE = sum((vMagDb - vHdB).^2) / length(vMagDb);

fprintf(['Convergence after ', num2str(output.iterations), ' iterations |']);
fprintf([' achieved MSE(H) = ', num2str(sMSE), 'dB\n']);

cLegend = cell(size(mTrue, 1) + 2, 1);
  cLegend{1} = 'Gemessen'; cLegend{2} = ['Schätzung (MSE(H) = ', num2str(sMSE), 'dB'];
figure('Units', 'Centimeters', 'Position', [52, 2, 24, 15]);
  semilogx(vF, vMagDb, 'x', vF, vHdB);
  hold on;
  for iE = 1:size(mTrue, 1)
    semilogx(vF, 20 * log10(abs(mH(:, iE))), '--');
    cLegend{iE + 2} = ['Glocke ', num2str(iE)];
  end
  hAx = gca;
    set(hAx.Children(end-1), 'LineWidth', 3);
  xlabel('Frequency in Hz'), ylabel('Magnitude in dB'), grid on;
  legend(cLegend, 'Location', 'southwest'), xlim([20 20e3]);


%% Functions
function [vH, vHdB] = func_peqtf(sA0, sW0, sQp, vS)
  if nargin == 3
    vS = logspace(log10(20), log10(20e3), 1e3);
  end
  
  vH = (vS.^2 + sA0 * sW0 / sQp * vS + sW0^2) ./ (vS.^2 + sW0 / sQp * vS + sW0^2);
  vHdB = 20 * log10(abs(vH));
end

function vHdB = func_MagFit(vS, vB0)
  nRuns = length(vB0) / 3;
  vHdB = zeros(size(vS));
  for iRun = 1:nRuns
    sW0 = 2 * pi * vB0(iRun);
    sA0 = 10^(vB0(iRun + nRuns) / 20);
    sQ0 = sqrt(2) * vB0(iRun + 2 * nRuns);
    [~, vHdBNew] = func_peqtf(sA0, sW0, sQ0, vS);
    vHdB = vHdB + vHdBNew;
  end
  
end