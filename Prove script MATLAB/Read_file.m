%% Import data from text file
% Script for importing data from the following text file:
%
%    filename: C:\Users\GGiac\Desktop\Giacomo\Ingegneria\Magistrale\Borsa di ricerca\Test Progetto SAM\DFT_50Hz.csv

%% Setup the Import Options and import the data
opts = delimitedTextImportOptions("NumVariables", 11);

% Specify range and delimiter
opts.DataLines = [4, Inf];%legge tutte le righe a partire dalla 4
%i dati partono dalla 4 riga prima ci sono solo unità di misura e nomi
opts.Delimiter = [" ", ";"];

% Specify column names and types
opts.VariableNames = ["Date", "Time", "Ampiezza", "Fase", "Frequenza", "Rocof", "Errore Ampiezza", "Errore Fase", "Errore Frequenza", "Errore Rocof", "Errore TVE"];
opts.SelectedVariableNames = ["Date", "Time", "Ampiezza", "Fase", "Frequenza", "Rocof", "Errore Ampiezza", "Errore Fase", "Errore Frequenza", "Errore Rocof", "Errore TVE"];
opts.VariableTypes = ["categorical", "categorical", "string", "string", "string", "string", "string", "string", "string", "string", "string"];
%ho deciso di metterli tutti stringhe perchè altrimenti mi approssima
%compleatamente il numero o al suo posto mi inseriva NaN
% Specify file level properties
opts.ExtraColumnsRule = "ignore";
opts.EmptyLineRule = "read";
opts.ConsecutiveDelimitersRule = "join";

% Specify variable properties
%opts = setvaropts(opts, ["Errore Ampiezza", "Errore Fase", "Errore Frequenza", "Errore Rocof", "Errore TVE"], "WhitespaceRule", "preserve");
opts = setvaropts(opts, ["Date", "Time"], "EmptyFieldRule", "auto");
%opts = setvaropts(opts, "Fase", "TrimNonNumeric", true);
%opts = setvaropts(opts, "Fase", "ThousandsSeparator", ",");

% Import the data
File = "DFT_45,0Hz_2CicliNom.csv"
Path = "C:\Users\GGiac\Desktop\Giacomo\Ingegneria\Magistrale\Borsa di ricerca\Test Progetto SAM\" + File
DFT50Hz = readtable(Path, opts);


%% Clear temporary variables
clear opts

%Date = DFT50Hz(1,1)
%Time = DFT50Hz(:,2)
%Time = table2array(Time).'%volevo provarlo anche così ma il grafico mi dà problemi
Data = DFT50Hz(:,3:11)
Frequenza = DFT50Hz(:,5)

tf = 1;%[s]
fs = 10000
i_sottocamp = 1
freq = fs/i_sottocamp
relative_time = 0:1/freq:tf;
N_camp = size(Frequenza)
N_camp = N_camp(1)
relative_time = relative_time(1:N_camp)
    
Frequenza = table2array(Frequenza).'
Frequenza = strrep(Frequenza, ',', '.');

Frequenza = str2double(Frequenza)
figure
plot(relative_time, Frequenza)
xlabel('Time')
ylabel('Frequenza [Hz]')
title('Frequenza a 45.0 Hz misurata con DFT a 2 Cicli Nominali')

Errore_Frequenza = DFT50Hz(:,9)
Errore_Frequenza = table2array(Errore_Frequenza).'
Errore_Frequenza = strrep(Errore_Frequenza, ',', '.');
Errore_Frequenza = str2double(Errore_Frequenza)

figure
plot(relative_time, Errore_Frequenza)
xlabel('Time')
ylabel('Errore frequenza [mHz]')
title('Errore frequenza a 45.0 Hz misurato con DFT a 2 Cicli Nominali')