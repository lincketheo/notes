# Changelog

## v1.0.0-wip CURRENT <tjl@rincon.com>

## Changed
- Refactor to XMidas option tree

## v0.0.3 2024-11-28 <tjl@rincon.com>

## Added 
- Create a new abstraction around FILE and xmidas pipe
- Refactors to C++
- `gen_some_sigs` - An extension of `gen_sigs`:
    - Adds support for calculating peak and "next peak" 
- Various plotting mechanisms in python
- Tests for full tx rx and channel chains

## Removed 

## v0.0.2 2024-11-21 <tjl@rincon.com>

## Added 
- Support for linear ramp up and down
- `gen_sigs` - A test to output many signal files of varying rutf

## Removed 
- Command line inputs for `tx` `rx` and `channel`. Restrict to a set of times

## v0.0.1 2024-11-12 <tjl@rincon.com>

## Added
- `dtob` - Converts data to a bluefile
- `fftcaf_lite` - runs fftcaf with low memory consumption and high fidelity
- `test` - runs tests 
- `tx` - Runs full tx with command line parameters
- `channel` - Runs full tx chain with command line parameters
- `rx` - Runs full rx chain with command line parameters
