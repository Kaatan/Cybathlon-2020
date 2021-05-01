package com.example.roboticarm;

import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.TextView;

import io.github.controlwear.virtual.joystick.android.JoystickView;

public class MainActivity extends AppCompatActivity {

    //déclaration des textView pour les informations du joystick
    private TextView mTextViewAngleRight;
    private TextView mTextViewStrengthRight;
    private TextView mTextViewCoordinateRight;

    //déclaration de la seekbar pour gérer la rotation du poignet
    private SeekBar seekBar_rotation;
    //déclaration du textview pour l'affichage de l'angle du poignet
    private TextView textView_angle;

    //déclaration du bouton pour réinitialiser les réglages
    private Button button_reset;

    //déclaration du bouton pour accéder aux réglages
    private Button button_reglage;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        //correspondance avec les textview dans activity_main.xml
        mTextViewAngleRight = (TextView) findViewById(R.id.textView_angle_right);
        mTextViewStrengthRight = (TextView) findViewById(R.id.textView_strength_right);
        mTextViewCoordinateRight = findViewById(R.id.textView_coordinate_right);

        //déclaration du joystick
        final JoystickView joystickRight = (JoystickView) findViewById(R.id.joystickView_right);
        joystickRight.setOnMoveListener(new JoystickView.OnMoveListener() {
            @SuppressLint("DefaultLocale")
            @Override
            public void onMove(int angle, int strength) {
                mTextViewAngleRight.setText(angle + "°");
                mTextViewStrengthRight.setText(strength + "%");
                mTextViewCoordinateRight.setText(
                        String.format("x%03d:y%03d",
                                joystickRight.getNormalizedX(),
                                joystickRight.getNormalizedY())
                );
            }
        });

        //correspondance avec les textview dans activity_main.xml
        seekBar_rotation = findViewById(R.id.seekBar_rotation);
        textView_angle = findViewById(R.id.textView_angle);
        button_reset = findViewById(R.id.button_reset);

        //fonction pour associer la position de la seekbar avec l'angle affiché
        seekBar_rotation.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener(){
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                textView_angle.setText(progress+"°");
            }
            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }
            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        //fonction pour reinitialiser la seekbar et l'affichage de l'angle
        button_reset.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v) {
                textView_angle.setText("0°");
                seekBar_rotation.setProgress(0);
            }
        });

        //association et fonction du bouton pour accéder aux réglages
        button_reglage = findViewById(R.id.button_reglage);
        button_reglage.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v) {
                openActivity_reglage();
            }
        });
    }

    //fonction pour accéder aux réglages
    public void openActivity_reglage(){
        Intent intent = new Intent(this, MainActivity_reglage.class);
        startActivity(intent);
    };

}