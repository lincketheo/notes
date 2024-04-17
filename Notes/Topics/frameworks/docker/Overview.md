# Architecture
![Docker Architecture](./assets/docker-architecture.webp "Docker Architecture")

## Docker Daemon
- `dockerd`
- Listens to docker api requests and manages docker objects.
- Can communicate with other daemons to manage Docker services

## Docker Client
- `$ docker`
- Primary way of interacting with docker
- Can communicate with multiple daemons

## Docker Desktop
- Contains:
  - Docker daemon
  - Docker client
  - Docker compose 
  - Docker Content trust
  - Kubernetes
  - Credential helper

## Docker registries
- Stores docker images
- E.g.
  - Docker hub - public registry anyone can use
- `docker run|pull`
  - Pulls required images from your registry
- `docker push` 
  - Pushes to configured registry

## Docker Objects
- **Images:**
  - Read only template with instructions for creating docker containers
  - Can be based on another image with additional customization (e.g. `ubuntu`)
  - Changing the dockerfile and rebuilding:
    - Only layers that are changed are rebuilt
- **Containers:**
  - A runnable instance of an image
  - Create, Stop, Start, Move, Delete
  - Well isolated from other containers 

# Technology
- Written in Go
- Uses linux kernel features 
- `namespaces`:
  - Provide isoolated workspace called a container