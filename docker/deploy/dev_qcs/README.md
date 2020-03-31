## Instructions for using this image 

SSH into your QMI instance and create an SSH tunnel mapping remote port 3000 to local port 8888 
```bash 
$ ssh -i YOUR_PRIVATE_KEY_FILE -L 8888:localhost:3000 forest@YOUR_QMI_IP
```

Once in, run the following command 

```bash
$ docker run --security-opt seccomp=unconfined --init -it \ 
             -p 3000:3000 --net=host -v "$(pwd)/.qcs:/root/.qcs" \
             -v "$(pwd)/.qcs_config:/root/.qcs_config" xacc/xacc-qcs
```

Open up `localhost:8888` to see an Eclipse Theia IDE with everything installed 
and ready to for using the XACC QCS Accelerator.