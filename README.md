# Distributed Learning Anomaly
This repository is to record history about tracking distributed deep learning locality anomalies.
<br>

# 1. Is it strictly epoch standard?
Log session hooks are called after `run`. But this `run` is not as same as `epoch`.<br>
Therefore, It cannot be explicitly said that even if `after_run` detects that epoch value>=10,<br>
It is not exactly epoch==10, but something like 10.02.
<br>

However, it looks like there is no need to worry about because between every run,<br>
Epoch is defined by global step, global batch size, and number of records,<br>
therefore, every epoch value `after_run` prints should be equal.<br>
However just in case, record start epoch value and start time,
and when epoch value is equal or bigger than the specific value,<br>
print differences in both epoch and time, and epoch difference divided by time difference.
<br>

# 2. Configure tensorflow not to save logs in /tmp
There is a suspicision that writing files of big size result in anomalies.
<br>

Even though `log_dir` is not set, the checkpoints are recorded while training.<br>
Inside the python script `/workspace/nvidia-examples/cnn/nvutils/runner.py`, within the `train` method,<br>
set `classifier`'s config related member variables to `None`.

Also, tensorflow uses a temporary model directory to write logs.<br>
This can be avoided by editing file:<br>
> /usr/local/lib/python3.6/dist-packages/tensorflow_estimator/python/estimator/estimator.py
<br>

At line number 1821, edit lines like this:
```python
  elif getattr(config, 'model_dir', None) is None:
    pass
    #model_dir = tempfile.mkdtemp()
    #logging.warning('Using temporary folder as model directory: %s', model_dir)
    #config = run_config.RunConfig.replace(config, model_dir=model_dir)
```
After this edit, I observed that temporary folder is not created inside `/tmp`.
However, will it really not write files by just letting the `model_dir` to be `None`?


# 3. Eliminate network part
`/usr/local/lib/python3.6/dist-packages/horovod/tensorflow/__init__.py`<br>
The above python script implements how ring-allreduce works.<br>
For example, see the line 311.
<br>

# 4. baidu-allreduce
Found a interesting repository on Github. The link is [here](https://github.com/baidu-research/baidu-allreduce).<br>
The repository is made by baidu, the company which implemented ring-allreduce algorithm.<br>
The repository is written in C++, and it demonstrates how the algorithm works in a brief.
<br>

I have run the code with two locality environments, 4-2-2, and 4-3-1.<br>
The three nodes are all in the same rack, so they share the same farthest network distance.<br>
Also, The node with the most processes has 4, which means they will have almost equal loads.
<br>

![image](img/210222-baidu-allreduce-Run-Time-locality-comparison-(8-GPUs).png)
<br>

Since ring-allreduce is irrelvant to number of nodes or process distribution across
nodes, the result is quite astonishing.<br>
It was seen that the node with only one process is the source of the problem.<br>
The implementation of scatter-reduce (first phase out of two phases of ring-allreduce) is as follows:
<br>

```cpp
for (int i = 0; i < size - 1; i++) {
  int recv_chunk = (rank - i - 1 + size) % size;
  int send_chunk = (rank - i + size) % size;
  float* segment_send = &(output[segment_ends[send_chunk] -
                              segment_sizes[send_chunk]]);


  MPI_Irecv(buffer, segment_sizes[recv_chunk],
          datatype, recv_from, 0, MPI_COMM_WORLD, &recv_req);


  MPI_Send(segment_send, segment_sizes[send_chunk],
          MPI_FLOAT, send_to, 0, MPI_COMM_WORLD);

  float *segment_update = &(output[segment_ends[recv_chunk] -
                                    segment_sizes[recv_chunk]]);

  // Wait for recv to complete before reduction
  MPI_Wait(&recv_req, &recv_status);

  reduce(segment_update, buffer, segment_sizes[recv_chunk]);

}
```
<br>

It is a mere of demonstration of ring-allreduce,<br>
and does not know whether there are changes applied to horovod, implementation,<br>
but horovod mentioned they replaced the implementation with NCCL, and made several API improvements.<br>
Take a look at the section 4 Installing horovod, in this [report](https://arxiv.org/pdf/1802.05799.pdf) written by horovod team.<br>
Right now, I could not find any actual implemented code with MPI APIs inside horovod repository.
<br>
