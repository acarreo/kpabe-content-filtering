
echo "Running benchmarks... for 100 iterations"

echo
echo "Running bench-params"
time ./bench-params &>> bench-params.txt
time ./bench-params-compress &>> bench-params-compress.txt


echo
echo "Running bench decryption key -- No compression"
time ./bench-dec-key 10 10 1     &>> bench-dec-key.txt
time ./bench-dec-key 100 10 1    &>> bench-dec-key.txt
time ./bench-dec-key 10 100 1    &>> bench-dec-key.txt
time ./bench-dec-key 100 100 1   &>> bench-dec-key.txt
time ./bench-dec-key 1000 100 1  &>> bench-dec-key.txt
time ./bench-dec-key 100 1000 1  &>> bench-dec-key.txt
time ./bench-dec-key 1000 1000 1 &>> bench-dec-key.txt

echo
echo "Running bench decryption key -- Compression"
time ./bench-dec-key-compress 10 10 1     &>> bench-dec-key-compress.txt
time ./bench-dec-key-compress 100 10 1    &>> bench-dec-key-compress.txt
time ./bench-dec-key-compress 10 100 1    &>> bench-dec-key-compress.txt
time ./bench-dec-key-compress 100 100 1   &>> bench-dec-key-compress.txt
time ./bench-dec-key-compress 1000 100 1  &>> bench-dec-key-compress.txt
time ./bench-dec-key-compress 100 1000 1  &>> bench-dec-key-compress.txt
time ./bench-dec-key-compress 1000 1000 1 &>> bench-dec-key-compress.txt


echo
echo "Running bench encryption - decryption -- No compression"
time ./bench-enc 10 10 10     &>> bench-enc.txt
time ./bench-enc 100 10 10    &>> bench-enc.txt
time ./bench-enc 10 100 10    &>> bench-enc.txt
time ./bench-enc 10 10 100    &>> bench-enc.txt
time ./bench-enc 100 100 10   &>> bench-enc.txt
time ./bench-enc 100 10 100   &>> bench-enc.txt
time ./bench-enc 10 100 100   &>> bench-enc.txt
time ./bench-enc 100 100 100  &>> bench-enc.txt

echo
echo "Running bench encryption - decryption LARGE PARAMETERS -- No compression"
time ./bench-enc 1000 100 100   &>> bench-enc-large.txt
time ./bench-enc 100 1000 100   &>> bench-enc-large.txt
time ./bench-enc 100 100 1000   &>> bench-enc-large.txt
time ./bench-enc 1000 1000 100  &>> bench-enc-large.txt
time ./bench-enc 1000 100 1000  &>> bench-enc-large.txt
time ./bench-enc 100 1000 1000  &>> bench-enc-large.txt
time ./bench-enc 1000 1000 1000 &>> bench-enc-large.txt


echo
echo "Running bench encryption - decryption -- Compression"
time ./bench-enc-compress 10 10 10     &>> bench-enc-compress.txt
time ./bench-enc-compress 100 10 10    &>> bench-enc-compress.txt
time ./bench-enc-compress 10 100 10    &>> bench-enc-compress.txt
time ./bench-enc-compress 10 10 100    &>> bench-enc-compress.txt
time ./bench-enc-compress 100 100 10   &>> bench-enc-compress.txt
time ./bench-enc-compress 100 10 100   &>> bench-enc-compress.txt
time ./bench-enc-compress 10 100 100   &>> bench-enc-compress.txt
time ./bench-enc-compress 100 100 100  &>> bench-enc-compress.txt

echo
echo "Running bench encryption - decryption LARGE PARAMETERS -- Compression"
time ./bench-enc-compress 1000 100 100   &>> bench-enc-large-compress.txt
time ./bench-enc-compress 100 1000 100   &>> bench-enc-large-compress.txt
time ./bench-enc-compress 100 100 1000   &>> bench-enc-large-compress.txt
time ./bench-enc-compress 1000 1000 100  &>> bench-enc-large-compress.txt
time ./bench-enc-compress 1000 100 1000  &>> bench-enc-large-compress.txt
time ./bench-enc-compress 100 1000 1000  &>> bench-enc-large-compress.txt
time ./bench-enc-compress 1000 1000 1000 &>> bench-enc-large-compress.txt


echo
echo "Benchmarks done!"
