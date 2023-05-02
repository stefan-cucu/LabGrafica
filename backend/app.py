import csv
import os
from flask import Flask, request, jsonify

app = Flask(__name__)

USER_FILE = 'users.csv'
SCORES_FILE = 'scores.csv'

# User login handler
@app.route('/login', methods=['POST'])
def login():
    username = request.json.get('username')
    password = request.json.get('password')

    # Check if user exists and password is correct
    if username == "" or password == "":
        return jsonify({'message': 'Invalid username or password'}), 409
    elif check_credentials(username, password):
        return jsonify({'message': 'Login successful'})
    else:
        return jsonify({'message': 'Invalid username or password'}), 401

# User signup handler
@app.route('/signup', methods=['POST'])
def signup():
    username = request.json.get('username')
    password = request.json.get('password')
    # Check if username is available
    if username == "" or password == "":
        return jsonify({'message': 'Invalid username or password'}), 409
    elif is_username_available(username):
        # Store user information
        store_user(username, password)
        return jsonify({'message': 'Signup successful'})
    else:
        return jsonify({'message': 'Username is already taken'}), 409

# Store user score handler
@app.route('/store_score', methods=['POST'])
def store_score():
    username = request.json.get('username')
    score = request.json.get('score')

    # Store user score
    save_score(username, score)
    return jsonify({'message': 'Score stored successfully'})

# Get top 10 scores handler
@app.route('/top_scores', methods=['GET'])
def top_scores():
    scores = get_top_scores()
    return jsonify(scores)

# Helper functions
def check_credentials(username, password):
    with open(USER_FILE, 'r') as file:
        reader = csv.reader(file)
        for row in reader:
            print(row)
            try:
                if row[0] == username and row[1] == password:
                    return True
            except:
                return False
    return False

def is_username_available(username):
    with open(USER_FILE, 'r') as file:
        reader = csv.reader(file)
        try:
            for row in reader:
                if row[0] == username:
                    return False
        except:
                return True
    return True

def store_user(username, password):
    with open(USER_FILE, 'a') as file:
        file.write('\n')
        writer = csv.writer(file)
        writer.writerow([username, password])

def save_score(username, score):
    scores = []
    updated = False

    with open(SCORES_FILE, 'r') as file:
        reader = csv.reader(file)
        for row in reader:
            try:
                if row[0] == username:
                    updated = True
                    if int(row[1]) <= int(score):
                        scores.append([username, score])
                    else:
                        scores.append(row)
                else:
                    scores.append(row)
            except:
                pass

    if not updated:
        scores.append([username, score])
    print(scores)
    with open(SCORES_FILE, 'w') as file:
        for row in scores:
            file.write(f'{row[0]},{row[1]}\n')


def get_top_scores():
    scores = []
    with open(SCORES_FILE, 'r') as file:
        reader = csv.reader(file)
        sorted_scores = sorted(reader, key=lambda row: int(row[1]), reverse=True)
        for row in sorted_scores[:10]:
            scores.append({'username': row[0], 'score': int(row[1])})
    return scores


@app.before_first_request
def create_empty_files():
    file1 = USER_FILE
    file2 = SCORES_FILE
    if not os.path.exists(file1):
        with open(file1, 'w'):
            pass
        print(f"{file1} created.")
    else:
        print(f"{file1} already exists.")

    if not os.path.exists(file2):
        with open(file2, 'w'):
            pass
        print(f"{file2} created.")
    else:
        print(f"{file2} already exists.")


# Start the Flask app
if __name__ == '__main__':
    app.run()
