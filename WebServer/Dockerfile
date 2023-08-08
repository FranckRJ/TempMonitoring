FROM python:3.11

RUN groupadd -r worker && useradd --no-log-init -r -m -s /bin/bash -g worker worker
USER worker:worker
ENV PATH="/home/worker/.local/bin:${PATH}"
WORKDIR /home/worker

COPY --chown=worker:worker requirements.txt requirements.txt
RUN pip install --no-cache-dir --user -r requirements.txt

COPY --chown=worker:worker . .

CMD ["python", "-m" , "flask", "run", "--host=0.0.0.0"]
