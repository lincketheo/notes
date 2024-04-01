https://doc.traefik.io/traefik/



# Maesh *
    - Lightweight and straightforward
    - Traefik instead of Envoy (open source)
    - No side car:
        - Proxy endpoints for each node
        - Non invasive - doesn't edit kubernetes objects
    - Configurations:
        - Annotations on user service objects
        - Service Mesh Interface objects
        - SMI = _good_

# Pros
    - Lightweight / easy to use 
    - Can be used with docker alone