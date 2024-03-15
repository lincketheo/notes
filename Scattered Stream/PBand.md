
TASK 1: Generate HTML Results for single tone test

- Post process single tone test
- Re run generate html file

## Post process test_single_tone_rx

```
ssh blvalab1
cd /warehouse/tjl/pband_testing/pband_testing_suite/
xmstart 
<cmd>
```

240314_184657/test_single_tone_rx => rxa-1
240313_112642/test_single_tone_rx => rxa-2
240313_160853/test_single_tone_rx => rxb-1
240314_122134/test_single_tone_rx => rxb-2
```
$ xmpy postprocess_single_tone_rx.py --data_root ./pband/716/240314_184657/test_single_tone_rx/ -c ./cfg/pband.yml --xcvr 0 --chan 1 --verbose # Running
$ xmpy postprocess_single_tone_rx.py --data_root ./pband/716/240313_112642/test_single_tone_rx/ -c ./cfg/pband.yml --xcvr 0 --chan 2 --verbose # 
$ xmpy postprocess_single_tone_rx.py --data_root ./pband/716/240313_160853/test_single_tone_rx/ -c ./cfg/pband.yml --xcvr 1 --chan 1 --verbose # 
$ xmpy postprocess_single_tone_rx.py --data_root ./pband/716/240314_122134/test_single_tone_rx/ -c ./cfg/pband.yml --xcvr 1 --chan 2 --verbose # 
```

## Testing 
```
$ xmpy postprocess_single_tone_rx.py --data_root ./pband/716/testing/test_single_tone_rx/ -c ./cfg/pband.yml --xcvr 0 --chan 1 --verbose
```

TASK 2: Tar up the tests (over night) (blvatjl)
- Do this at night on blvatjl

# Tasks
- Add boolean to run psa on each file or not
- Turn it off and re run

# Problem:
- Runs psa.py on each file (takes a long time)
- Disabled this by adding a boolean, but the next line depends on the std out of 

[23][019][0-9]