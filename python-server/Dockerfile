# Gunakan imej Python yang ringan (slim)

FROM python:3.9-slim



# Tetapkan folder kerja di dalam container

WORKDIR /app



# --- UPDATE: Install library MySQL connector ---

# Ini wajib ada supaya python boleh bercakap dengan database

RUN pip install mysql-connector-python



# Salin kod server ke dalam container

COPY server.py .



# Dedahkan port 8080 (sama seperti dalam kod Python)

EXPOSE 8080



# Jalankan server tanpa buffering supaya log print terus keluar di terminal Docker

CMD ["python", "-u", "server.py"]



