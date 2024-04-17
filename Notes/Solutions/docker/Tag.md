- `docker image tag <src> <destination>`
- Tags:
  - `[HOST[:PORT_NUMBER]/]PATH`
- e.g.:
  - Pull an image then tag it for local registry:
```
docker pull ubuntu
docker tag ubuntu localhost:5000/ubuntu
docker push localhost:5000/ubuntu
```
