# Develop with Theia

To develop XACC using the Eclipse Theia IDE and Docker

```bash
$ git clone --recursive https://github.com/eclipse/xacc
$ cd xacc/docker/dev/theia
$ docker-compose up -d
```

Navigate to `http://localhost:3000` in your web browser.

To delete this development workspace
```bash
$ docker-compose down
```