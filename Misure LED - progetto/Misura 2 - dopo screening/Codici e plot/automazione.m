nled = 35;
a='';

for cnt = 1:nled
    a='I'+string(cnt)+'_c = I'+string(cnt)+'_b(floor(size(I'+string(cnt)+'_b,1)/2):end);'
end