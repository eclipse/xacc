# Develop with Theia

To develop XACC using the Eclipse Theia IDE and Docker

```bash
$ git clone --recursive https://github.com/eclipse/xacc
$ cd xacc/docker/dev/theia
$ docker-compose up -d
```

Navigate to `http://localhost:3000` in your web browser. For an application look and feel in Google Chrome, you can run 
```bash
$ /Applications/Google\ Chrome.app/Contents/MacOS/Google\ Chrome --app=http://localhost:3000 (On a Mac)
$ google-chrome --app=http://localhost:3000 (On Linux)
```

To delete this development workspace
```bash
$ docker-compose down
```
