# locality
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

speed 2.0091563113145847 25.99881935119629 0.07727875193771586
speed 2.0091563113145847 26.132620334625244 0.07688307891009649
speed 2.0091563113145847 26.301871061325073 0.0763883415985906