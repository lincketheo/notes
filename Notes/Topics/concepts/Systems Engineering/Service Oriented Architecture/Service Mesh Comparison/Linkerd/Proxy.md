# Linkerd2-proxy

## Notes:
- Speed:
    - Envoy: 172 C++ KLOC - complexity score of 19k
    - Linkerd2-proxy 30 KLOC - complexity score of 1.5k Proxy Used:
    - Low memory usage:
    > Linkerd2-proxy instances consistently between 14mb and 15mb of memory, while Istio’s Envoy ranged between 135mb and 175mb—ten times the size. Similarly, Linkerd2-proxy’s CPU usage for the test run was consistently at 15ms (CPU milliseconds) per instance, while Istio’s Envoy ranged from 22ms to 156ms—from 50% more to 10x more.

- Security:
    - They think rust offers advantages to C++ in security


- Linkerd doesn’t use Envoy because using Envoy wouldn’t allow us to build the lightest, simplest, and most secure Kubernetes service mesh in the world.