FROM python:3.9-slim
WORKDIR /app
COPY client.py .
# '-u' supaya print output terus keluar di log Docker tanpa buffer
CMD ["python", "-u", "client.py"]