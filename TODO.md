# TODO List
- [ ] Need to optimize WaveArray samples method
- [ ] Extensive validation of from_samples and interfere methods
- [ ] Make WaveArray from samples fft faster by only computing wave rows that will be kept by the amplitude filtering
- [x] make to_csv and samples_to_csv use the eigen utils save_to_csv
- [x] make from_csv use the eigen utils load_from_csv
- [x] ensure from_csv raises error if header not correct order freq, amp, phase
- [x] update save_to_csv util to use path, not string
